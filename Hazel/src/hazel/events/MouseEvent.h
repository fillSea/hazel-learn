#pragma once

#include <sstream>

#include "Event.h"

namespace hazel {
// 鼠标移动事件
class HAZEL_API MouseMovedEvent : public Event {
public:
    MouseMovedEvent(float x, float y) : mouse_x_(x), mouse_y_(y) {}

    inline float getX() const { return mouse_x_; }
    inline float getY() const { return mouse_y_; }

    std::string toString() const override {
        std::stringstream ss;
        ss << "MouseMovedEvent: " << mouse_x_ << ", " << mouse_y_;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
private:
    float mouse_x_, mouse_y_;  // 鼠标位置
};

// 鼠标滚动事件
class HAZEL_API MouseScrolledEvent : public Event {
public:
    MouseScrolledEvent(float xOffset, float yOffset) : x_offset_(xOffset), y_offset_(yOffset) {}

    inline float getXOffset() const { return x_offset_; }
    inline float getYOffset() const { return y_offset_; }

    std::string toString() const override {
        std::stringstream ss;
        ss << "MouseScrolledEvent: " << getXOffset() << ", " << getYOffset();
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseScrolled)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
private:
    float x_offset_, y_offset_;  // 滚动偏移量
};

// 鼠标按钮事件基类
class HAZEL_API MouseButtonEvent : public Event {
public:
    inline int getMouseButton() const { return button_; }

    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
protected:
    MouseButtonEvent(int button) : button_(button) {}

    int button_;  // 鼠标按钮
};

// 鼠标按钮按下事件
class HAZEL_API MouseButtonPressedEvent : public MouseButtonEvent {
public:
    MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

    std::string toString() const override {
        std::stringstream ss;
        ss << "MouseButtonPressedEvent: " << button_;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseButtonPressed)
};

// 鼠标按钮释放事件
class HAZEL_API MouseButtonReleasedEvent : public MouseButtonEvent {
public:
    MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

    std::string toString() const override {
        std::stringstream ss;
        ss << "MouseButtonReleasedEvent: " << button_;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseButtonReleased)
};
}  // namespace hazel