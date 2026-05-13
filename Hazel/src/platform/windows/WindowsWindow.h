#pragma once

#include "hazel/Window.h"

class GLFWwindow;

namespace hazel {
class WindowsWindow : public Window {
public:
	WindowsWindow(const WindowProps& props);
	virtual ~WindowsWindow();

	void onUpdate() override;

	inline unsigned int getWidth() const override { return data_.width; }
	inline unsigned int getHeight() const override { return data_.height; }

	// Window attributes
	inline void setEventCallback(const EventCallbackFn& callback) override { data_.event_callback = callback; }
	void setVSync(bool enabled) override;
	bool isVSync() const override;

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