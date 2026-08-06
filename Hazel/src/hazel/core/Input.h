#pragma once
#include "hazel/core/Core.h"
#include "hazel/core/KeyCodes.h"
#include "hazel/core/MouseCodes.h"

namespace hazel {
class HAZEL_API Input {
public:
	inline static bool isKeyPressed(KeyCode key) { return instance_->isKeyPressedImpl(key); }

	inline static bool isMouseButtonPressed(MouseCode button) { return instance_->isMouseButtonPressedImpl(button); }
	inline static std::pair<float, float> getMousePosition() { return instance_->getMousePositionImpl(); }
	inline static float getMouseX() { return instance_->getMouseXImpl(); }
	inline static float getMouseY() { return instance_->getMouseYImpl(); }

protected:
	virtual bool isKeyPressedImpl(KeyCode key) = 0;

	virtual bool isMouseButtonPressedImpl(MouseCode button) = 0;
	virtual std::pair<float, float> getMousePositionImpl() = 0;
	virtual float getMouseXImpl() = 0;
	virtual float getMouseYImpl() = 0;

private:
	static Scope<Input> instance_;
};
}  // namespace hazel