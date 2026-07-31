#include "WindowsInput.h"

#include <GLFW/glfw3.h>

#include "hazel/core/Application.h"

namespace hazel {
Scope<Input> Input::instance_ = createScope<WindowsInput>();

bool WindowsInput::isKeyPressedImpl(int keycode) {
	auto* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
	auto state = glfwGetKey(window, keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool WindowsInput::isMouseButtonPressedImpl(int button) {
	auto* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
	auto state = glfwGetMouseButton(window, button);
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