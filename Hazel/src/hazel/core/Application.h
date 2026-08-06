#pragma once
#include "hazel/ImGui/ImGuiLayer.h"
#include "hazel/core/Core.h"
#include "hazel/core/Layer.h"
#include "hazel/core/LayerStack.h"
#include "hazel/core/Window.h"
#include "hazel/events/ApplicationEvent.h"

int main(int argc, char** argv);

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

	void onEvent(Event& e);

	void pushLayer(Layer* layer);
	void pushOverlay(Layer* overlay);

	inline Window& getWindow() { return *window_; }

	inline static Application& getInstance() { return *instance_; }

private:
	void run();
	bool onWindowClose(WindowCloseEvent& e);
	bool onWindowResize(WindowResizeEvent& e);

private:
	friend int ::main(int argc, char** argv);

	static Application* instance_;

	std::unique_ptr<Window> window_;

	ImGuiLayer* imGui_layer_;
	LayerStack layer_stack_;

	bool running_{true};
	bool minimized_{false};

	float last_frame_time_{0.0f};  ///< 上一帧的时间戳
};
// 应该在客户端实现
Application* createApplication();
}  // namespace hazel