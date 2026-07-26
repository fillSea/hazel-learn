#pragma once

#include <functional>
#include <string>

#include "hazel/core/Core.h"
#include "hazel/events/Event.h"

namespace hazel {
// 窗口属性
struct WindowProps {
	std::string title;
	unsigned int width;
	unsigned int height;

	explicit WindowProps(const std::string& title = "Hazel Engine", unsigned int width = 1280,
	                     unsigned int height = 720)
	    : title(title), width(width), height(height) {}
};

// Interface representing a desktop system based Window
class HAZEL_API Window {
public:
	using EventCallbackFn = std::function<void(Event&)>;

	virtual ~Window() {}

	virtual void onUpdate() = 0;

	virtual unsigned int getWidth() const = 0;
	virtual unsigned int getHeight() const = 0;

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
	static Window* create(const WindowProps& props = WindowProps());
};
}  // namespace hazel