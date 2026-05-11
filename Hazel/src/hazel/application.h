#pragma once
#include "Core.h"
#include "hazel/Window.h"
#include "hazel/events/ApplicationEvent.h"

namespace hazel {
class HAZEL_API Application {
public:
	Application();
	virtual ~Application();
	void run();

	void onEvent(Event& e);

private:
	bool onWindowClose(WindowCloseEvent& e);

private:
	std::unique_ptr<Window> window_;
	bool running_{true};
};
// 应该在客户端实现
Application* createApplication();
}  // namespace hazel