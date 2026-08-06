#include "WindowsWindow.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "hazel/events/ApplicationEvent.h"
#include "hazel/events/KeyEvent.h"
#include "hazel/events/MouseEvent.h"
#include "hazel/renderer/GraphicsContext.h"
#include "platform/OpenGL/OpenGLContext.h"

namespace hazel {
// 静态全局变量
// 避免在多个线程中初始化 GLFW
static uint8_t g_s_glfw_window_count = 0;

static void onGLFWError(int error, const char* description) {
	HZ_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

WindowsWindow::WindowsWindow(const WindowProps& props) {
	HZ_PROFILE_FUNCTION();

	init(props);
}

WindowsWindow::~WindowsWindow() {
	HZ_PROFILE_FUNCTION();

	shutdown();
}

void WindowsWindow::init(const WindowProps& props) {
	HZ_PROFILE_FUNCTION();

	data_.title = props.title;
	data_.width = props.width;
	data_.height = props.height;

	HZ_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

	if (g_s_glfw_window_count == 0) {
		HZ_PROFILE_SCOPE("glfwInit");

		HZ_CORE_INFO("Initializing GLFW");
		int success = glfwInit();
		HZ_CORE_ASSERT(success, "Could not intialize GLFW!");
		glfwSetErrorCallback(onGLFWError);
	}

	{
		HZ_PROFILE_SCOPE("glfwCreateWindow");
		window_ = glfwCreateWindow(static_cast<int>(props.width), static_cast<int>(props.height), props.title.c_str(),
		                           nullptr, nullptr);
		++g_s_glfw_window_count;
	}

	context_ = createScope<OpenGLContext>(window_);
	context_->init();

	// 设置窗口用户指针
	glfwSetWindowUserPointer(window_, &data_);
	setVSync(true);

	// Set GLFW callbacks
	glfwSetWindowSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data.width = width;
		data.height = height;

		WindowResizeEvent event(width, height);
		data.event_callback(event);
	});

	glfwSetWindowCloseCallback(window_, [](GLFWwindow* window) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		WindowCloseEvent event;
		data.event_callback(event);
	});

	glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		switch (action) {
			case GLFW_PRESS: {
				KeyPressedEvent event(static_cast<KeyCode>(key), 0);
				data.event_callback(event);
				break;
			}
			case GLFW_RELEASE: {
				KeyReleasedEvent event(static_cast<KeyCode>(key));
				data.event_callback(event);
				break;
			}
			case GLFW_REPEAT: {
				KeyPressedEvent event(static_cast<KeyCode>(key), 1);
				data.event_callback(event);
				break;
			}
		}
	});

	glfwSetCharCallback(window_, [](GLFWwindow* window, unsigned int keycode) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		KeyTypedEvent event(static_cast<KeyCode>(keycode));
		data.event_callback(event);
	});

	glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		switch (action) {
			case GLFW_PRESS: {
				MouseButtonPressedEvent event(static_cast<MouseCode>(button));
				data.event_callback(event);
				break;
			}
			case GLFW_RELEASE: {
				MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
				data.event_callback(event);
				break;
			}
		}
	});

	glfwSetScrollCallback(window_, [](GLFWwindow* window, double x_offset, double y_offset) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		MouseScrolledEvent event(static_cast<float>(x_offset), static_cast<float>(y_offset));
		data.event_callback(event);
	});

	glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double x_pos, double y_pos) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		MouseMovedEvent event(static_cast<float>(x_pos), static_cast<float>(y_pos));
		data.event_callback(event);
	});
}

void WindowsWindow::shutdown() {
	HZ_PROFILE_FUNCTION();

	glfwDestroyWindow(window_);

	if (--g_s_glfw_window_count == 0) {
		HZ_CORE_INFO("Terminating GLFW");
		glfwTerminate();
	}
}

void WindowsWindow::onUpdate() {
	HZ_PROFILE_FUNCTION();

	glfwPollEvents();
	context_->swapBuffers();
}

void WindowsWindow::setVSync(bool enabled) {
	HZ_PROFILE_FUNCTION();

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
