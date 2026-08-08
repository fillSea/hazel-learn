#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <string>
#include <thread>

#include "hazel/core/Log.h"

namespace hazel {
using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

// 单次函数/代码段的性能采样结果，对应 Chrome Tracing 中的一条 trace event
struct ProfileResult {
	std::string name;  // 被采样对象的名称（函数名或自定义名称）
	FloatingPointMicroseconds start;
	std::chrono::microseconds elapsed_time;
	std::thread::id thread_id;  // 执行该代码段的线程 ID（std::thread::id 的哈希值）
};

// 一次性能分析会话，对应输出 JSON 文件中的一次会话
struct InstrumentationSession {
	std::string name;  // 会话名称
};

// Instrumentor:性能分析器(单例),负责管理分析会话,并将采样结果
// 以 Chrome Tracing 的 JSON 格式写入文件(可直接在 chrome://tracing 中查看)
class Instrumentor {
private:
	std::mutex mutex_;

	InstrumentationSession* current_session_{};  // 当前进行中的会话(无会话时为 nullptr)
	std::ofstream output_stream_;                // 结果输出文件流

public:
	Instrumentor() = default;

	static Instrumentor& getInstance() {
		static Instrumentor s_instance;
		return s_instance;
	}

	// 开启分析会话:创建/覆盖输出文件、写入 JSON 头部并创建会话对象
	void beginSession(const std::string& name, const std::string& filepath = "results.json") {
		std::lock_guard lock(mutex_);
		if (current_session_) {
			// If there is already a current session, then close it before beginning new one.
			// Subsequent profiling output meant for the original session will end up in the
			// newly opened session instead.  That's better than having badly formatted
			// profiling output.
			if (Log::getCoreLogger()) {  // Edge case: BeginSession() might be before Log::Init()
				HZ_CORE_ERROR("Instrumentor::BeginSession('{0}') when session '{1}' already open.", name,
				              current_session_->name);
			}
			endSession();
		}

		output_stream_.open(filepath);

		if (output_stream_.is_open()) {
			current_session_ = new InstrumentationSession({name});
			writeHeader();
		} else {
			if (Log::getCoreLogger()) {  // Edge case: BeginSession() might be before Log::Init()
				HZ_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
			}
		}
	}

	// 结束当前会话:写入 JSON 尾部、关闭文件并释放会话内存,重置计数器
	void endSession() {
		std::lock_guard lock(mutex_);
		internalEndSession();
	}

	// 将一条采样结果序列化为 trace event 追加写入文件
	// (名称中的双引号会被替换为单引号以保证 JSON 合法性)
	void writeProfile(const ProfileResult& result) {
		std::stringstream json;

		std::string name = result.name;
		std::replace(name.begin(), name.end(), '"', '\'');

		json << std::setprecision(3) << std::fixed;
		json << "{";
		json << R"("cat":"function",)";
		json << "\"dur\":" << (result.elapsed_time.count()) << ',';
		json << R"("name":")" << name << "\",";
		json << R"("ph":"X",)";
		json << "\"pid\":0,";
		json << "\"tid\":" << result.thread_id << ",";
		json << "\"ts\":" << result.start.count();
		json << "}";

		std::lock_guard lock(mutex_);
		if (current_session_) {
			output_stream_ << json.str();
			output_stream_.flush();
		}
	}

	// 写入 JSON 头部:traceEvents 数组开始
	void writeHeader() {
		output_stream_ << R"({"otherData": {},"traceEvents":[{})";
		output_stream_.flush();
	}

	// 写入 JSON 尾部:闭合 traceEvents 数组与根对象
	void writeFooter() {
		output_stream_ << "]}";
		output_stream_.flush();
	}

	// Note: you must already own lock on m_Mutex before
	// calling InternalEndSession()
	void internalEndSession() {
		if (current_session_) {
			writeFooter();
			output_stream_.close();
			delete current_session_;
			current_session_ = nullptr;
		}
	}
};

// InstrumentationTimer:基于 RAII 的作用域计时器。
// 构造时记录起始时间点;作用域退出或显式调用 stop() 时记录结束时间,
// 以微秒精度与当前线程 ID 生成 ProfileResult 并交给 Instrumentor 写入。
// 禁止拷贝/移动,保证一个计时器实例只能采样一次
class InstrumentationTimer {
public:
	// 以采样名称构造并开始计时
	explicit InstrumentationTimer(const char* name) : name_(name) {
		start_timepoint_ = std::chrono::steady_clock::now();
	}

	// 若尚未停止则自动调用 stop(),确保作用域退出时一定完成采样
	~InstrumentationTimer() {
		if (!stopped_) {
			stop();
		}
	}

	InstrumentationTimer(const InstrumentationTimer&) = delete;
	InstrumentationTimer& operator=(const InstrumentationTimer&) = delete;
	InstrumentationTimer(InstrumentationTimer&&) = delete;
	InstrumentationTimer& operator=(InstrumentationTimer&&) = delete;

	// 停止计时并提交采样结果(幂等:重复调用会被 stopped_ 标志拦截)
	void stop() {
		auto end_timepoint = std::chrono::steady_clock::now();
		auto high_res_start = FloatingPointMicroseconds{start_timepoint_.time_since_epoch()};
		auto elapsed_time =
		    std::chrono::time_point_cast<std::chrono::microseconds>(end_timepoint).time_since_epoch() -
		    std::chrono::time_point_cast<std::chrono::microseconds>(start_timepoint_).time_since_epoch();

		uint32_t thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
		Instrumentor::getInstance().writeProfile({name_, high_res_start, elapsed_time, std::this_thread::get_id()});

		stopped_ = true;
	}

private:
	const char* name_;                                                             // 采样名称(通常为函数名或自定义标识)
	std::chrono::time_point<std::chrono::high_resolution_clock> start_timepoint_;  // 计时起点
	bool stopped_{};                                                               // 是否已停止采样
};
}  // namespace hazel

#define HZ_PROFILE 0
#if HZ_PROFILE
#define HZ_PROFILE_BEGIN_SESSION(name, filepath) ::hazel::Instrumentor::getInstance().beginSession(name, filepath)
#define HZ_PROFILE_END_SESSION() ::hazel::Instrumentor::getInstance().endSession()
#define HZ_PROFILE_SCOPE(name) ::hazel::InstrumentationTimer timer##__LINE__(name);
#define HZ_PROFILE_FUNCTION() HZ_PROFILE_SCOPE(__FUNCSIG__)
#else
#define HZ_PROFILE_BEGIN_SESSION(name, filepath)
#define HZ_PROFILE_END_SESSION()
#define HZ_PROFILE_SCOPE(name)
#define HZ_PROFILE_FUNCTION()
#endif