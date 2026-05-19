#pragma once

#include "hazel/Layer.h"
#include "hazel/events/ApplicationEvent.h"
#include "hazel/events/Event.h"
#include "hazel/events/KeyEvent.h"
#include "hazel/events/MouseEvent.h"

namespace hazel {
class HAZEL_API ImGuiLayer : public Layer {
public:
	ImGuiLayer();
	~ImGuiLayer() override = default;

	ImGuiLayer(const ImGuiLayer&) = delete;
	ImGuiLayer& operator=(const ImGuiLayer&) = delete;
	ImGuiLayer(ImGuiLayer&&) = delete;
	ImGuiLayer& operator=(ImGuiLayer&&) = delete;

	void onAttach() override;
	void onDetach() override;
	void onUpdate() override;
	void onEvent(Event& event) override;

private:
	bool onMouseButtonPressedEvent(MouseButtonPressedEvent& e);
	bool onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
	bool onMouseMovedEvent(MouseMovedEvent& e);
	bool onMouseScrolledEvent(MouseScrolledEvent& e);
	bool onKeyPressedEvent(KeyPressedEvent& e);
	bool onKeyReleasedEvent(KeyReleasedEvent& e);
	bool onKeyTypedEvent(KeyTypedEvent& e);
	bool onWindowResizeEvent(WindowResizeEvent& e);

private:
	// 时间戳, 用于帧同步
	float time_ = 0.0F;
};
}  // namespace hazel