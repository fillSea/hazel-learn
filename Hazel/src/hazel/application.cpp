#include "Application.h"

#include <GLFW/glfw3.h>

#include <memory>

namespace hazel {
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

Application* Application::instance_ = nullptr;

Application::Application() {
	HZ_CORE_ASSERT(!instance_, "Application already exists!");
	instance_ = this;

	window_ = std::unique_ptr<Window>(Window::create());
	window_->setEventCallback(BIND_EVENT_FN(Application::onEvent));

	imGui_layer_ = new ImGuiLayer();
	pushOverlay(imGui_layer_);
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
		// 计算时间步长，用于帧同步
		float time = static_cast<float>(glfwGetTime());
		Timestep timestep(time - last_frame_time_);
		last_frame_time_ = time;

		for (Layer* layer : layer_stack_) {
			layer->onUpdate(timestep);
		}

		imGui_layer_->begin();
		for (Layer* layer : layer_stack_) {
			layer->onImGuiRender();
		}
		imGui_layer_->end();

		window_->onUpdate();
	}
}

bool Application::onWindowClose(WindowCloseEvent& e) {
	running_ = false;
	return true;
}
}  // namespace hazel