#pragma once
#include <functional>
#include <string>

#include "../Core.h"

namespace hazel {
// Events in Hazel are currently blocking, meaning when an event occurs it
// immediately gets dispatched and must be dealt with right then an there.
// For the future, a better strategy might be to buffer events in an event
// bus and process them during the "event" part of the update stage.

// 事件类型
enum class EventType {
    None = 0,
    // 窗口事件
    WindowClose,      // 窗口关闭事件
    WindowResize,     // 窗口调整大小事件
    WindowFocus,      // 窗口获得焦点事件
    WindowLostFocus,  // 窗口失去焦点事件
    WindowMoved,      // 窗口移动事件
    // 应用事件
    AppTick,    // 帧事件
    AppUpdate,  // 更新事件
    AppRender,  // 渲染事件
    // 键盘事件
    KeyPressed,   // 键盘按下事件
    KeyReleased,  // 键盘释放事件
    // 鼠标事件
    MouseButtonPressed,   // 鼠标按钮按下事件
    MouseButtonReleased,  // 鼠标按钮释放事件
    MouseMoved,           // 鼠标移动事件
    MouseScrolled,        // 鼠标滚动轮事件
};

// 事件分类
enum EventCategory {
    None = 0,
    EventCategoryApplication = BIT(0),  // 应用事件分类
    EventCategoryInput = BIT(1),        // 输入事件分类
    EventCategoryKeyboard = BIT(2),     // 键盘事件分类
    EventCategoryMouse = BIT(3),        // 鼠标事件分类
    EventCategoryMouseButton = BIT(4)   // 鼠标按钮事件分类
};

// 事件类型宏
// 实现获取事件类型
#define EVENT_CLASS_TYPE(type)                \
    static EventType getStaticType() {        \
        return EventType::type;               \
    }                                         \
    EventType getEventType() const override { \
        return getStaticType();               \
    }                                         \
    const char* getName() const override {    \
        return #type;                         \
    }

// 事件分类宏
// 实现获取事件分类
#define EVENT_CLASS_CATEGORY(category)      \
    int getCategoryFlags() const override { \
        return category;                    \
    }

// 事件基类
class HAZEL_API Event {
    friend class EventDispatcher;

public:
    virtual EventType getEventType() const = 0;
    virtual const char* getName() const = 0;
    virtual int getCategoryFlags() const = 0;
    virtual std::string toString() const { return getName(); }

    // 是否属于指定分类
    inline bool isInCategory(EventCategory category) { return getCategoryFlags() & category; }

protected:
    bool handled_ = false;  // 事件是否已被处理
};

// 事件分发器
class EventDispatcher {
    template <typename T>
    using EventFunction = std::function<bool(T&)>;

public:
    EventDispatcher(Event& event) : event_(event) {}

    // 分发事件
    template <typename T>
    bool dispatch(EventFunction<T> func) {
        // 检查事件类型是否匹配
        if (event_.getEventType() == T::getStaticType()) {
            // 将基类引用转为具体类型引用, 并调用处理函数
            event_.handled_ = func(*(T*) &event_);
            return true;
        }
        return false;
    }

private:
    Event& event_;  // 事件引用
};

inline std::ostream& operator<<(std::ostream& os, const Event& e) {
    return os << e.toString();
}
}  // namespace hazel