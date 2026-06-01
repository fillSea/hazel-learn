#pragma once

#include "hazel/Window.h"

class GLFWwindow;

namespace hazel {
class WindowsWindow : public Window {
public:
	explicit WindowsWindow(const WindowProps& props);
	~WindowsWindow() override;
	WindowsWindow(const WindowsWindow&) = delete;
	WindowsWindow& operator=(const WindowsWindow&) = delete;
	WindowsWindow(WindowsWindow&&) = delete;
	WindowsWindow& operator=(WindowsWindow&&) = delete;

	void onUpdate() override;

	inline unsigned int getWidth() const override { return data_.width; }
	inline unsigned int getHeight() const override { return data_.height; }

	// Window attributes
	inline void setEventCallback(const EventCallbackFn& callback) override { data_.event_callback = callback; }
	void setVSync(bool enabled) override;
	bool isVSync() const override;

	inline void* getNativeWindow() const override { return window_; }

private:
	virtual void init(const WindowProps& props);
	virtual void shutdown();

private:
	GLFWwindow* window_;  // GLFW 窗口指针

	// 窗口数据
	struct WindowData {
		std::string title;
		unsigned int width, height;
		bool vsync;  // 是否开启垂直同步

		EventCallbackFn event_callback;
	};

	WindowData data_;
};
}  // namespace hazel