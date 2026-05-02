#pragma once
#include <sstream>

#include "Event.h"

namespace hazel {
// 窗口调整大小事件
class HAZEL_API WindowResizeEvent : public Event {
public:
    WindowResizeEvent(unsigned int width, unsigned int height) : width_(width), height_(height) {}

    inline unsigned int getWidth() const { return width_; }
    inline unsigned int getHeight() const { return height_; }

    std::string toString() const override {
        std::stringstream ss;
        ss << "WindowResizeEvent: " << width_ << ", " << height_;
        return ss.str();
    }

    EVENT_CLASS_TYPE(WindowResize)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
private:
    unsigned int width_, height_;  // 窗口宽度和高度
};

// 窗口关闭事件
class HAZEL_API WindowCloseEvent : public Event {
public:
    WindowCloseEvent() = default;

    EVENT_CLASS_TYPE(WindowClose)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

// 应用帧事件
class HAZEL_API AppTickEvent : public Event {
public:
    AppTickEvent() = default;

    EVENT_CLASS_TYPE(AppTick)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

// 应用更新事件
class HAZEL_API AppUpdateEvent : public Event {
public:
    AppUpdateEvent() = default;

    EVENT_CLASS_TYPE(AppUpdate)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

// 应用渲染事件
class HAZEL_API AppRenderEvent : public Event {
public:
    AppRenderEvent() = default;

    EVENT_CLASS_TYPE(AppRender)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};
}  // namespace hazel