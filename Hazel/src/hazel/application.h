#pragma once
#include "Core.h"
#include "hazel/ImGui/ImGuiLayer.h"
#include "hazel/Layer.h"
#include "hazel/LayerStack.h"
#include "hazel/Window.h"
#include "hazel/events/ApplicationEvent.h"

namespace hazel {
class HAZEL_API Application {
public:
	Application();
	virtual ~Application();

	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator=(Application&&) = delete;

	void run();

	void onEvent(Event& e);

	void pushLayer(Layer* layer);
	void pushOverlay(Layer* overlay);

	inline Window& getWindow() { return *window_; }

	inline static Application& getInstance() { return *instance_; }

private:
	bool onWindowClose(WindowCloseEvent& e);

private:
	std::unique_ptr<Window> window_;
	ImGuiLayer* imGui_layer_;
	bool running_{true};
	LayerStack layer_stack_;
	static Application* instance_;
};
// 应该在客户端实现
Application* createApplication();
}  // namespace hazel