// clang-format off
#include "hzpch.h"
// clang-format on
#include "WindowsWindow.h"

namespace hazel {
// 静态全局变量
// 避免在多个线程中初始化 GLFW
static bool g_s_glfw_initialized = false;

Window* Window::create(const WindowProps& props) {
	return new WindowsWindow(props);
}

WindowsWindow::WindowsWindow(const WindowProps& props) {
	init(props);
}

WindowsWindow::~WindowsWindow() {
	shutdown();
}

void WindowsWindow::init(const WindowProps& props) {
	data_.title = props.title;
	data_.width = props.width;
	data_.height = props.height;

	HZ_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

	if (!g_s_glfw_initialized) {
		// TODO: glfwTerminate on system shutdown
		int success = glfwInit();
		HZ_CORE_ASSERT(success, "Could not intialize GLFW!");

		g_s_glfw_initialized = true;
	}

	window_ = glfwCreateWindow(static_cast<int>(props.width), static_cast<int>(props.height), props.title.c_str(),
	                           nullptr, nullptr);
	// 设置当前线程的 GLFW 上下文为窗口上下文
	glfwMakeContextCurrent(window_);
	// 设置窗口用户指针
	glfwSetWindowUserPointer(window_, &data_);
	setVSync(true);
}

void WindowsWindow::shutdown() {
	glfwDestroyWindow(window_);
}

void WindowsWindow::onUpdate() {
	glfwPollEvents();
	glfwSwapBuffers(window_);
}

void WindowsWindow::setVSync(bool enabled) {
	if (enabled) {
		// 开启垂直同步
		glfwSwapInterval(1);
	} else {
		glfwSwapInterval(0);
	}

	data_.vsync = enabled;
}

bool WindowsWindow::isVSync() const {
	return data_.vsync;
}
}  // namespace hazel