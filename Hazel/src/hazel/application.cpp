// clang-format off
#include "hzpch.h"
// clang-format on
#include "Application.h"

#include "events/ApplicationEvent.h"
#include "events/Event.h"
#include "hazel/Log.h"

namespace hazel {
Application::Application() = default;
Application::~Application() = default;
void Application::run() {
	WindowResizeEvent e(1280, 720);
	if (e.isInCategory(EventCategoryApplication)) {
		// 原来：HZ_TRACE(e);
		HZ_TRACE("{0}", e.toString());
	}
	if (e.isInCategory(EventCategoryInput)) {
		HZ_TRACE("{0}", e.toString());
	}
	while (true)
		;
}
}  // namespace hazel
