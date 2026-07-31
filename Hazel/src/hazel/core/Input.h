#pragma once
#include "hazel/core/Core.h"

namespace hazel {
class HAZEL_API Input {
public:
	inline static bool isKeyPressed(int keycode) { return instance_->isKeyPressedImpl(keycode); }

	inline static bool isMouseButtonPressed(int button) { return instance_->isMouseButtonPressedImpl(button); }
	inline static std::pair<float, float> getMousePosition() { return instance_->getMousePositionImpl(); }
	inline static float getMouseX() { return instance_->getMouseXImpl(); }
	inline static float getMouseY() { return instance_->getMouseYImpl(); }

protected:
	virtual bool isKeyPressedImpl(int keycode) = 0;

	virtual bool isMouseButtonPressedImpl(int button) = 0;
	virtual std::pair<float, float> getMousePositionImpl() = 0;
	virtual float getMouseXImpl() = 0;
	virtual float getMouseYImpl() = 0;

private:
	static Scope<Input> instance_;
};
}  // namespace hazel