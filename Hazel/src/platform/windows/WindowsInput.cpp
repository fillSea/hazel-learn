#include <GLFW/glfw3.h>

#include "hazel/core/Application.h"
#include "hazel/core/Input.h"

namespace hazel {
bool Input::isKeyPressed(KeyCode key) {
	auto* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
	auto state = glfwGetKey(window, static_cast<int32_t>(key));
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::isMouseButtonPressed(MouseCode button) {
	auto* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
	auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
	return state == GLFW_PRESS;
}

std::pair<float, float> Input::getMousePosition() {
	auto* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	return {static_cast<float>(xpos), static_cast<float>(ypos)};
}

float Input::getMouseX() {
	auto [x, y] = getMousePosition();
	return x;
}

float Input::getMouseY() {
	auto [x, y] = getMousePosition();
	return y;
}
}  // namespace hazel