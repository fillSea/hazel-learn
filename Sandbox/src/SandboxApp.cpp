#include <Hazel.h>

class ExampleLayer : public hazel::Layer {
public:
	ExampleLayer() : Layer("Example") {}

	void onUpdate() override {
		if (hazel::Input::isKeyPressed(HZ_KEY_TAB)) {
			HZ_TRACE("Tab key is pressed (poll)!");
		}
	}
	void onEvent(hazel::Event& event) override {
		if (event.getEventType() == hazel::EventType::KeyPressed) {
			auto& e = dynamic_cast<hazel::KeyPressedEvent&>(event);
			if (e.getKeyCode() == HZ_KEY_TAB) {
				HZ_TRACE("Tab key is pressed (event)!");
			}
			HZ_TRACE("{0}", (char) e.getKeyCode());
		}
		HZ_TRACE("{0}", event.toString());
	}
};

class Sandbox : public hazel::Application {
public:
	Sandbox() {
		pushLayer(new ExampleLayer());
		pushOverlay(new hazel::ImGuiLayer());
	}
	~Sandbox() = default;
};

hazel::Application* hazel::createApplication() {
	return new Sandbox();
}