// clang-format off
#include "hzpch.h"
// clang-format on
#include "Application.h"

#include <GLFW/glfw3.h>
#include <gl/GL.h>

namespace hazel {
Application::Application() {
	window_ = std::unique_ptr<Window>(Window::create());
}

Application::~Application() = default;

void Application::run() {
	while (running_) {
		glClearColor(1, 0, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		window_->onUpdate();
	}
}
}  // namespace hazel
