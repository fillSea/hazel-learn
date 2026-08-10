#include "Application.h"

#include <GLFW/glfw3.h>

#include <memory>

#include "hazel/renderer/Renderer.h"

namespace hazel {
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

Application* Application::instance_ = nullptr;

Application::Application() {
	HZ_PROFILE_FUNCTION();

	HZ_CORE_ASSERT(!instance_, "Application already exists!");
	instance_ = this;

	window_ = std::unique_ptr<Window>(Window::create());
	window_->setEventCallback(BIND_EVENT_FN(Application::onEvent));

	Renderer::init();

	imGui_layer_ = new ImGuiLayer();
	pushOverlay(imGui_layer_);
}

Application::~Application() {
	HZ_PROFILE_FUNCTION();

	Renderer::shutdown();
}

void Application::pushLayer(Layer* layer) {
	HZ_PROFILE_FUNCTION();

	layer_stack_.pushLayer(layer);

	layer->onAttach();
}

void Application::pushOverlay(Layer* overlay) {
	HZ_PROFILE_FUNCTION();

	layer_stack_.pushOverlay(overlay);

	overlay->onAttach();
}

void Application::onEvent(Event& e) {
	HZ_PROFILE_FUNCTION();

	EventDispatcher dispatcher(e);
	dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::onWindowClose));
	dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::onWindowResize));

	// 从后往前遍历，先处理上层的事件
	for (auto it = layer_stack_.end(); it != layer_stack_.begin();) {
		// 如果事件被处理，就跳出循环
		if (e.isHandled()) {
			break;
		}
		(*--it)->onEvent(e);
	}
}

void Application::run() {
	HZ_PROFILE_FUNCTION();

	while (running_) {
		HZ_PROFILE_SCOPE("RunLoop");

		// 计算时间步长，用于帧同步
		float time = static_cast<float>(glfwGetTime());
		Timestep timestep(time - last_frame_time_);
		last_frame_time_ = time;

		if (!minimized_) {
			{
				HZ_PROFILE_SCOPE("LayerStack OnUpdate");

				for (Layer* layer : layer_stack_) {
					layer->onUpdate(timestep);
				}
			}
		}

		imGui_layer_->begin();
		{
			HZ_PROFILE_SCOPE("LayerStack OnImGuiRender");

			for (Layer* layer : layer_stack_) {
				layer->onImGuiRender();
			}
		}
		imGui_layer_->end();

		window_->onUpdate();
	}
}

bool Application::onWindowClose(WindowCloseEvent& e) {
	running_ = false;
	return true;
}

bool Application::onWindowResize(WindowResizeEvent& e) {
	HZ_PROFILE_FUNCTION();

	if (e.getWidth() == 0 || e.getHeight() == 0) {
		minimized_ = true;
		return false;
	}

	minimized_ = false;
	Renderer::onWindowResize(e.getWidth(), e.getHeight());

	return false;
}

void Application::close() {
	running_ = false;
}
}  // namespace hazel