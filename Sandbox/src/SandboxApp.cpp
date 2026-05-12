#include <Hazel.h>

class ExampleLayer : public hazel::Layer {
public:
	ExampleLayer() : Layer("Example") {}

	void onUpdate() override { HZ_INFO("ExampleLayer::Update"); }
	void onEvent(hazel::Event& event) override { HZ_TRACE("{0}", event.toString()); }
};

class Sandbox : public hazel::Application {
public:
	Sandbox() { pushLayer(new ExampleLayer()); }
	~Sandbox() = default;
};

hazel::Application* hazel::createApplication() {
	return new Sandbox();
}