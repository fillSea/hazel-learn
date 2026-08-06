#pragma once
#include "hazel/core/Input.h"

namespace hazel {
class WindowsInput : public Input {
protected:
	bool isKeyPressedImpl(KeyCode key) override;
	bool isMouseButtonPressedImpl(MouseCode button) override;
	std::pair<float, float> getMousePositionImpl() override;
	float getMouseXImpl() override;
	float getMouseYImpl() override;
};
}  // namespace hazel