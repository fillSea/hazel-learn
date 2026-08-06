#pragma once

#include "hazel/core/KeyCodes.h"
#include "hazel/events/Event.h"

namespace hazel {
// 键盘事件基类
class HAZEL_API KeyEvent : public Event {
public:
	inline KeyCode getKeyCode() const { return key_code_; }

	EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
protected:
	explicit KeyEvent(KeyCode key_code) : key_code_(key_code) {}

	KeyCode key_code_;  // 键盘码
};

// 键盘按下事件
class HAZEL_API KeyPressedEvent : public KeyEvent {
public:
	KeyPressedEvent(KeyCode keycode, int repeat_count) : KeyEvent(keycode), repeat_count_(repeat_count) {}

	inline int getRepeatCount() const { return repeat_count_; }

	std::string toString() const override {
		std::stringstream ss;
		ss << "KeyPressedEvent: " << key_code_ << " (" << repeat_count_ << " repeats)";
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyPressed)
private:
	int repeat_count_;  // 按下重复次数
};

// 键盘释放事件
class HAZEL_API KeyReleasedEvent : public KeyEvent {
public:
	explicit KeyReleasedEvent(KeyCode keycode) : KeyEvent(keycode) {}

	std::string toString() const override {
		std::stringstream ss;
		ss << "KeyReleasedEvent: " << key_code_;
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyReleased)
};

class HAZEL_API KeyTypedEvent : public KeyEvent {
public:
	explicit KeyTypedEvent(KeyCode keycode) : KeyEvent(keycode) {}

	std::string toString() const override {
		std::stringstream ss;
		ss << "KeyTypedEvent: " << key_code_;
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyTyped)
};
}  // namespace hazel