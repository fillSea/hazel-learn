#include <Hazel.h>

class Sandbox : public hazel::Application {
public:
	Sandbox() = default;
	~Sandbox() = default;
};

hazel::Application* hazel::createApplication() {
	return new Sandbox();
}