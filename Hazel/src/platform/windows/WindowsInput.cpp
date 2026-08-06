#include "WindowsInput.h"

#include <GLFW/glfw3.h>

#include "hazel/core/Application.h"

namespace hazel {
bool WindowsInput::isKeyPressedImpl(KeyCode key) {
	auto* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
	auto state = glfwGetKey(window, static_cast<int32_t>(key));
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool WindowsInput::isMouseButtonPressedImpl(MouseCode button) {
	auto* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
	auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
	return state == GLFW_PRESS;
}

std::pair<float, float> WindowsInput::getMousePositionImpl() {
	auto* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	return {static_cast<float>(xpos), static_cast<float>(ypos)};
}

float WindowsInput::getMouseXImpl() {
	auto [x, y] = getMousePositionImpl();
	return x;
}

float WindowsInput::getMouseYImpl() {
	auto [x, y] = getMousePositionImpl();
	return y;
}
}  // namespace hazel