#pragma once

#include <functional>
#include <string>

#include "hazel/core/Core.h"
#include "hazel/events/Event.h"

namespace hazel {
// 窗口属性
struct WindowProps {
	std::string title;
	uint32_t width;
	uint32_t height;

	explicit WindowProps(const std::string& title = "Hazel Engine", uint32_t width = 1280, uint32_t height = 720)
	    : title(title), width(width), height(height) {}
};

// Interface representing a desktop system based Window
class HAZEL_API Window {
public:
	using EventCallbackFn = std::function<void(Event&)>;

	virtual ~Window() = default;

	virtual void onUpdate() = 0;

	virtual uint32_t getWidth() const = 0;
	virtual uint32_t getHeight() const = 0;

	// Window attributes
	// 设置事件回调函数
	virtual void setEventCallback(const EventCallbackFn& callback) = 0;
	// 设置垂直同步
	virtual void setVSync(bool enabled) = 0;
	// 是否开启垂直同步
	virtual bool isVSync() const = 0;

	// 获取窗口句柄
	virtual void* getNativeWindow() const = 0;

	// 创建窗口
	static Scope<Window> create(const WindowProps& props = WindowProps());
};
}  // namespace hazel