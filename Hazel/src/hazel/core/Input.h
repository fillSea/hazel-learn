#pragma once
#include "hazel/core/Core.h"
#include "hazel/core/KeyCodes.h"
#include "hazel/core/MouseCodes.h"

namespace hazel {
class HAZEL_API Input {
public:
	static bool isKeyPressed(KeyCode key);

	static bool isMouseButtonPressed(MouseCode button);
	static std::pair<float, float> getMousePosition();
	static float getMouseX();
	static float getMouseY();
};
}  // namespace hazel