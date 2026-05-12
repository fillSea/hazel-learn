#include "Application.h"

#include <GLFW/glfw3.h>
#include <gl/GL.h>

#include "hazel/Log.h"

namespace hazel {
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

Application::Application() {
	window_ = std::unique_ptr<Window>(Window::create());
	window_->setEventCallback(BIND_EVENT_FN(Application::onEvent));
}

Application::~Application() = default;

void Application::pushLayer(Layer* layer) {
	layer_stack_.pushLayer(layer);
}

void Application::pushOverlay(Layer* overlay) {
	layer_stack_.pushOverlay(overlay);
}

void Application::onEvent(Event& e) {
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::onWindowClose));

	// 从后往前遍历，先处理上层的事件
	for (auto it = layer_stack_.end(); it != layer_stack_.begin();) {
		(*--it)->onEvent(e);
		// 如果事件被处理，就跳出循环
		if (e.isHandled()) {
			break;
		}
	}
}

void Application::run() {
	while (running_) {
		glClearColor(1, 0, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		for (Layer* layer : layer_stack_) {
			layer->onUpdate();
		}

		window_->onUpdate();
	}
}

bool Application::onWindowClose(WindowCloseEvent& e) {
	running_ = false;
	return true;
}
}  // namespace hazel
