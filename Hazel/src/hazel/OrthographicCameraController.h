#pragma once

#include "hazel/core/Timestep.h"
#include "hazel/events/ApplicationEvent.h"
#include "hazel/events/MouseEvent.h"
#include "hazel/renderer/OrthographicCamera.h"

namespace hazel {

/// @brief 正交相机控制器，负责处理正交相机的移动、缩放、旋转等操作。
/// 通过键盘 WASD 移动相机，QE 旋转，鼠标滚轮缩放，并响应窗口大小变化。
class HAZEL_API OrthographicCameraController {
public:
	/// @brief 构造正交相机控制器。
	/// @param aspect_ratio 窗口的宽高比。
	/// @param rotation 是否启用相机旋转功能，默认为 false。
	explicit OrthographicCameraController(float aspect_ratio, bool rotation = false);

	/// @brief 每帧更新相机状态，处理键盘输入。
	/// @param ts 帧时间步长。
	void onUpdate(Timestep ts);

	/// @brief 处理事件，包括鼠标滚轮和窗口大小变化。
	/// @param e 待处理的事件。
	void onEvent(Event& e);

	/// @brief 获取正交相机引用（可修改）。
	/// @return 正交相机的非 const 引用。
	OrthographicCamera& getCamera() { return camera_; }

	/// @brief 获取正交相机 const 引用（只读）。
	/// @return 正交相机的 const 引用。
	[[nodiscard]] const OrthographicCamera& getCamera() const { return camera_; }

	[[nodiscard]] float getZoomLevel() const { return zoom_level_; }
	void setZoomLevel(float level) { zoom_level_ = level; }

private:
	/// @brief 处理鼠标滚轮事件，用于缩放相机。
	/// @param e 鼠标滚轮事件。
	/// @return 事件是否已处理。
	bool onMouseScrolled(MouseScrolledEvent& e);

	/// @brief 处理窗口大小变化事件，更新相机投影矩阵。
	/// @param e 窗口大小变化事件。
	/// @return 事件是否已处理。
	bool onWindowResized(WindowResizeEvent& e);

private:
	float aspect_ratio_;         ///< 窗口宽高比。
	float zoom_level_{1.0f};     ///< 当前缩放级别，默认 1.0。
	OrthographicCamera camera_;  ///< 正交相机实例。

	bool rotation_;  ///< 是否启用旋转功能。

	glm::vec3 camera_position_{0.0f, 0.0f, 0.0f};  ///< 相机在世界空间中的位置。
	float camera_rotation_{0.0f};                  ///< 相机旋转角度（度）。
	float camera_translation_speed_{5.0f};         ///< 相机移动速度（单位/秒）。
	float camera_rotation_speed_{180.0f};          ///< 相机旋转速度（度/秒）。
};

}  // namespace hazel