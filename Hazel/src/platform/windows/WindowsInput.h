#pragma once
#include "hazel/core/Input.h"

namespace hazel {
class WindowsInput : public Input {
protected:
	bool isKeyPressedImpl(int keycode) override;
	bool isMouseButtonPressedImpl(int button) override;
	std::pair<float, float> getMousePositionImpl() override;
	float getMouseXImpl() override;
	float getMouseYImpl() override;
};
}  // namespace hazel