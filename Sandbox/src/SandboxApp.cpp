#include <Hazel.h>
#include <hazel/core/EntryPoint.h>

#include "Sandbox2D.h"

class Sandbox : public hazel::Application {
public:
	Sandbox() { pushLayer(new Sandbox2D()); }
	~Sandbox() override = default;

	Sandbox(const Sandbox&) = delete;
	Sandbox& operator=(const Sandbox&) = delete;
	Sandbox(Sandbox&&) = delete;
	Sandbox& operator=(Sandbox&&) = delete;
};

hazel::Application* hazel::createApplication() {
	return new Sandbox();
}