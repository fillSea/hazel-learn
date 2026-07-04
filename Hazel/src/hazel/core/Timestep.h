#pragma once

namespace hazel {

/**
 * @brief 时间步长类，用于表示帧间隔时间
 *
 * 封装了以秒为单位的时间值，方便在引擎中以统一的方式
 * 传递和使用帧间隔时间（delta time）。
 */
class Timestep {
public:
	/**
	 * @brief 构造函数
	 * @param time 时间值，单位为秒，默认为 0
	 */
	explicit Timestep(float time = 0.0f) : time_(time) {};

	/// 隐式转换为 float，返回以秒为单位的时间值
	operator float() const { return time_; }

	/// 获取以秒为单位的时间值
	[[nodiscard]] float getSeconds() const { return time_; }
	/// 获取以毫秒为单位的时间值
	[[nodiscard]] float getMilliseconds() const { return time_ * 1000.0f; }

private:
	float time_;  ///< 以秒为单位存储的时间值
};

}  // namespace hazel