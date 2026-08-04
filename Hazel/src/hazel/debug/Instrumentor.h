#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <string>
#include <thread>

namespace hazel {
// 单次函数/代码段的性能采样结果，对应 Chrome Tracing 中的一条 trace event
struct ProfileResult {
	std::string name;    // 被采样对象的名称（函数名或自定义名称）
	int64_t start;       // 采样开始时间（微秒，自 Unix 纪元起）
	int64_t end;         // 采样结束时间（微秒，自 Unix 纪元起）
	uint32_t thread_id;  // 执行该代码段的线程 ID（std::thread::id 的哈希值）
};

// 一次性能分析会话，对应输出 JSON 文件中的一次会话
struct InstrumentationSession {
	std::string name;  // 会话名称
};

// Instrumentor:性能分析器(单例),负责管理分析会话,并将采样结果
// 以 Chrome Tracing 的 JSON 格式写入文件(可直接在 chrome://tracing 中查看)
class Instrumentor {
private:
	InstrumentationSession* current_session_{};  // 当前进行中的会话(无会话时为 nullptr)
	std::ofstream output_stream_;                // 结果输出文件流
	int profile_count_{};                        // 已写入的采样条数,用于 JSON 数组元素间的逗号分隔

public:
	Instrumentor() = default;

	// 开启分析会话:创建/覆盖输出文件、写入 JSON 头部并创建会话对象
	void beginSession(const std::string& name, const std::string& filepath = "results.json") {
		output_stream_.open(filepath);
		writeHeader();
		current_session_ = new InstrumentationSession{name};
	}

	// 结束当前会话:写入 JSON 尾部、关闭文件并释放会话内存,重置计数器
	void endSession() {
		writeFooter();
		output_stream_.close();
		delete current_session_;
		current_session_ = nullptr;
		profile_count_ = 0;
	}

	// 将一条采样结果序列化为 trace event 追加写入文件
	// (名称中的双引号会被替换为单引号以保证 JSON 合法性)
	void writeProfile(const ProfileResult& result) {
		if (profile_count_++ > 0) {
			output_stream_ << ",";
		}

		std::string name = result.name;
		std::replace(name.begin(), name.end(), '"', '\'');

		output_stream_ << "{";
		output_stream_ << R"("cat":"function",)";
		output_stream_ << "\"dur\":" << (result.end - result.start) << ',';
		output_stream_ << R"("name":")" << name << "\",";
		output_stream_ << R"("ph":"X",)";
		output_stream_ << "\"pid\":0,";
		output_stream_ << "\"tid\":" << result.thread_id << ",";
		output_stream_ << "\"ts\":" << result.start;
		output_stream_ << "}";

		output_stream_.flush();
	}

	// 写入 JSON 头部:traceEvents 数组开始
	void writeHeader() {
		output_stream_ << R"({"otherData": {},"traceEvents":[)";
		output_stream_.flush();
	}

	// 写入 JSON 尾部:闭合 traceEvents 数组与根对象
	void writeFooter() {
		output_stream_ << "]}";
		output_stream_.flush();
	}

	// 获取全局唯一实例
	static Instrumentor& get() {
		static Instrumentor s_instance;
		return s_instance;
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
		start_timepoint_ = std::chrono::high_resolution_clock::now();
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
		auto end_timepoint = std::chrono::high_resolution_clock::now();

		int64_t start =
		    std::chrono::time_point_cast<std::chrono::microseconds>(start_timepoint_).time_since_epoch().count();
		int64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(end_timepoint).time_since_epoch().count();

		uint32_t thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
		Instrumentor::get().writeProfile({name_, start, end, thread_id});

		stopped_ = true;
	}

private:
	const char* name_;                                                                   // 采样名称(通常为函数名或自定义标识)
	std::chrono::time_point<std::chrono::high_resolution_clock> start_timepoint_;        // 计时起点
	bool stopped_{};                                                                     // 是否已停止采样
};
}  // namespace hazel

#define HZ_PROFILE 1
#if HZ_PROFILE
#define HZ_PROFILE_BEGIN_SESSION(name, filepath) ::hazel::Instrumentor::get().beginSession(name, filepath)
#define HZ_PROFILE_END_SESSION() ::hazel::Instrumentor::get().endSession()
#define HZ_PROFILE_SCOPE(name) ::hazel::InstrumentationTimer timer##__LINE__(name);
#define HZ_PROFILE_FUNCTION() HZ_PROFILE_SCOPE(__FUNCSIG__)
#else
#define HZ_PROFILE_BEGIN_SESSION(name, filepath)
#define HZ_PROFILE_END_SESSION()
#define HZ_PROFILE_SCOPE(name)
#define HZ_PROFILE_FUNCTION()
#endif