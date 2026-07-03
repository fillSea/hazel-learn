#pragma once
#include "Core.h"
#include "hazel/ImGui/ImGuiLayer.h"
#include "hazel/Layer.h"
#include "hazel/LayerStack.h"
#include "hazel/Window.h"
#include "hazel/events/ApplicationEvent.h"
#include "hazel/renderer/OrthographicCamera.h"
#include "hazel/renderer/Shader.h"

namespace hazel {
class VertexBuffer;
class IndexBuffer;
class VertexArray;

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
	static Application* instance_;

	std::unique_ptr<Window> window_;

	std::shared_ptr<VertexArray> vertex_array_;

	std::shared_ptr<Shader> shader_;

	std::shared_ptr<Shader> blue_shader_;
	std::shared_ptr<VertexArray> square_va_;

	OrthographicCamera camera_;

	ImGuiLayer* imGui_layer_;
	LayerStack layer_stack_;

	bool running_{true};
};
// 应该在客户端实现
Application* createApplication();
}  // namespace hazel