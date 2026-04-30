#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace hazel {
std::shared_ptr<spdlog::logger> Log::core_logger_;
std::shared_ptr<spdlog::logger> Log::client_logger_;

void Log::init() {
    // 设置日志格式: [时间] 日志级别 日志名称: 日志消息
    spdlog::set_pattern("%^[%T] %n: %v%$");
    core_logger_ = spdlog::stdout_color_mt("Hazel");
    core_logger_->set_level(spdlog::level::trace);

    client_logger_ = spdlog::stdout_color_mt("APP");
    client_logger_->set_level(spdlog::level::trace);
}
}  // namespace hazel