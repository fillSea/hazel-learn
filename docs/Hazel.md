# 1. 项目结构
引擎：Hazel，编译为 `.dll` 动态库。
客户端程序：Sandbox，实现客户端的相关显示。
# 2. 入口点设置
## 2.1. core.h
该文件用于定义相关宏，以便简化导出代码的编写。
```cpp
#ifdef HZ_PLATFORM_WINDOWS
    #ifdef HZ_BUILD_DLL
        #define HAZEL_API __declspec(dllexport)
    #else
        #define HAZEL_API __declspec(dllimport)
    #endif
#else
    #define HAZEL_API
#endif
```
在 `CMakeLists.txt` 中定义宏：
```
target_compile_definitions(Hazel
    PUBLIC
        HZ_PLATFORM_WINDOWS
    PRIVATE
        HZ_BUILD_DLL
)
```
## 2.2. entry_point.h
该文件用于定义入口函数，当执行时，通过该文件进行启动。
```cpp
#ifdef HZ_PLATFORM_WINDOWS
extern Hazel::Application* Hazel::createApplication();

int main(int argc, char** argv) {
    auto app = Hazel::createApplication();
    app->run();
    delete app;

    return 0;
}

#endif
```
## 2.3. Application
实现应用程序基类。
```cpp
namespace Hazel {
class HAZEL_API Application {
public:
    Application();
    virtual ~Application();
    void run();
};
// 应该在客户端实现
Application* createApplication();
}  // namespace Hazel
```
## 2.4. Sandbox
继承 Application，实现具体的启动代码。
```cpp
class Sandbox : public Hazel::Application {
public:
    Sandbox() {}
    ~Sandbox() {}
};

Hazel::Application* Hazel::createApplication() {
    return new Sandbox();
}
```
## 2.5. 启动流程
```
┌─────────────────────────────────────────────────────────────┐
│                    编译阶段                                  │
├─────────────────────────────────────────────────────────────┤
│  CMake 检测平台 → 定义 HZ_PLATFORM_WINDOWS                  │
└─────────────────────────┬───────────────────────────────────┘
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                    预处理阶段                                │
├─────────────────────────────────────────────────────────────┤
│  #ifdef HZ_PLATFORM_WINDOWS → 条件编译生效                   │
│  main() 函数被包含进编译单元                                │
└─────────────────────────┬───────────────────────────────────┘
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                    链接阶段                                  │
├─────────────────────────────────────────────────────────────┤
│  extern 声明 → 链接器查找 createApplication() 实现           │
│  Sandbox.exe 提供该函数的实现                               │
└─────────────────────────┬───────────────────────────────────┘
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                    运行阶段                                  │
├─────────────────────────────────────────────────────────────┤
│  main() → createApplication() → Application::run()         │
└─────────────────────────┬───────────────────────────────────┘
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                    Sandbox.exe 启动                          │
└─────────────────────────┬───────────────────────────────────┘
                          ▼
┌─────────────────────────────────────────────────────────────┐
│  执行 entry_point.h 中的 main() 函数                        │
│  int main(int argc, char** argv) {                         │
│      auto app = Hazel::createApplication();  // ① 调用工厂函数│
│      app->run();                             // ② 进入主循环   │
│      delete app;                             // ③ 清理资源     │
│      return 0;                                           │
│  }                                                         │
└─────────────────────────┬───────────────────────────────────┘
                          ▼
┌─────────────────────────────────────────────────────────────┐
│  执行 Sandbox 中实现的 createApplication()                   │
│  Hazel::Application* Hazel::createApplication() {           │
│      return new Sandbox();  // 创建具体应用实例              │
│  }                                                         │
└─────────────────────────┬───────────────────────────────────┘
                          ▼
┌─────────────────────────────────────────────────────────────┐
│  执行 Application::run() 主循环                              │
│  void Application::run() {                                 │
│      while (true);  // 游戏主循环（目前是死循环）            │
│  }                                                         │
└─────────────────────────────────────────────────────────────┘
```
# 3. 日志
使用 spdlog 库进行日志记录。
需要将相关代码进行封装，这样修改使用的日志库时，无需修改上层代码。
## 3.1. Log 类
### 3.1.1. 定义
```cpp
namespace Hazel {
class HAZEL_API Log {
public:
    static void init();

    inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return core_logger_; }
    inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return client_logger_; }

private:
    static std::shared_ptr<spdlog::logger> core_logger_;
    static std::shared_ptr<spdlog::logger> client_logger_;
};
}  // namespace Hazel
```
### 3.1.2. 实现
```cpp
namespace Hazel {
std::shared_ptr<spdlog::logger> Log::core_logger_;
std::shared_ptr<spdlog::logger> Log::client_logger_;

void Log::init() {
    // 设置日志格式: [时间] 日志级别 日志名称: 日志消息
    spdlog::set_pattern("%^[%T] %n: %v%$");
    core_logger_ = spdlog::stdout_color_mt("Hazel");
    core_logger_->set_level(spdlog::level::trace);

    client_logger_ = spdlog::stdout_color_mt("APP");
    client_logger_->set_level(spdlog::level::trace);
}
}  // namespace Hazel
```
## 3.2. 宏定义
使用宏定义简化代码。
```cpp
// Core log macros
#define HZ_CORE_TRACE(...) ::Hazel::Log::getCoreLogger()->trace(__VA_ARGS__)
#define HZ_CORE_INFO(...) ::Hazel::Log::getCoreLogger()->info(__VA_ARGS__)
#define HZ_CORE_WARN(...) ::Hazel::Log::getCoreLogger()->warn(__VA_ARGS__)
#define HZ_CORE_ERROR(...) ::Hazel::Log::getCoreLogger()->error(__VA_ARGS__)
#define HZ_CORE_FATAL(...) ::Hazel::Log::getCoreLogger()->fatal(__VA_ARGS__)
// Client log macros
#define HZ_TRACE(...) ::Hazel::Log::getClientLogger()->trace(__VA_ARGS__)
#define HZ_INFO(...) ::Hazel::Log::getClientLogger()->info(__VA_ARGS__)
#define HZ_WARN(...) ::Hazel::Log::getClientLogger()->warn(__VA_ARGS__)
#define HZ_ERROR(...) ::Hazel::Log::getClientLogger()->error(__VA_ARGS__)
#define HZ_FATAL(...) ::Hazel::Log::getClientLogger()->fatal(__VA_ARGS__)
```
# 4. 事件系统
## 4.1. 基本类型定义
定义了事件类型和事件分类
```cpp
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
```
## 4.2. 事件基类
```cpp
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
```
所有的事件都继承自该基类。
为了代码编写，定义相关宏：
```cpp
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
```
## 4.3. 事件示例
事件定义如下所示，
```cpp
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
```
## 4.4. 事件分发器
```cpp
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
```
根据事件类型匹配对应事件，若匹配成功，将基类转换成派生类，调用回调函数，进行事件分发。
# 5. 窗口系统
## 5.1. 窗口属性
```cpp
// 窗口属性
struct WindowProps {
	std::string title;
	unsigned int width;
	unsigned int height;

	explicit WindowProps(const std::string& title = "Hazel Engine", unsigned int width = 1280,
	                     unsigned int height = 720)
	    : title(title), width(width), height(height) {}
};
```
## 5.2. 窗口基类
窗口中可以设置事件回调函数，包含窗口更新函数，可以设置每一帧窗口更新的内容。
```cpp
class HAZEL_API Window {
public:
	using EventCallbackFn = std::function<void(Event&)>;

	virtual ~Window() {}

	virtual void onUpdate() = 0;

	virtual unsigned int getWidth() const = 0;
	virtual unsigned int getHeight() const = 0;

	// Window attributes
	// 设置事件回调函数
	virtual void setEventCallback(const EventCallbackFn& callback) = 0;
	// 设置垂直同步
	virtual void setVSync(bool enabled) = 0;
	// 是否开启垂直同步
	virtual bool isVSync() const = 0;

	// 创建窗口
	static Window* create(const WindowProps& props = WindowProps());
};
```
## 5.3. Windows 窗口
该类实现了一个基本的 Windows，用来显示程序的主窗口。
基本结构如下：
```cpp
class WindowsWindow : public Window {
	//...
private:
	GLFWwindow* window_;  // GLFW 窗口指针

	// 窗口数据
	struct WindowData {
		std::string title;
		unsigned int width, height;
		bool vsync;  // 是否开启垂直同步

		EventCallbackFn event_callback;
	};

	WindowData data_;
};
```
该类设置 OpenGL 初始化的相关代码：
```cpp
window_ = glfwCreateWindow(static_cast<int>(props.width), static_cast<int>(props.height), props.title.c_str(),
	                           nullptr, nullptr);
	// 设置当前线程的 GLFW 上下文为窗口上下文
	glfwMakeContextCurrent(window_);
	// 设置窗口用户指针
	glfwSetWindowUserPointer(window_, &data_);
```
在 `onUpdate()` 中调用 OpenGL 的更新代码。
```cpp
void WindowsWindow::onUpdate() {
	glfwPollEvents();
	glfwSwapBuffers(window_);
}
```
## 5.4. 事件注册
设置了窗口的回调函数之后，就可以通过获取窗口数据调用相应的回调函数进行处理。
例如，
```cpp
glfwSetWindowSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data.width = width;
		data.height = height;

		WindowResizeEvent event(width, height);
		data.event_callback(event);
	});
```
# 6. Layer 层
Layer 层用于把应用中的功能模块拆分成独立对象，例如业务逻辑层、调试 UI 层、ImGui 层等。每个 Layer 都可以参与生命周期、每帧更新和事件处理，`Application` 通过 `LayerStack` 统一管理这些 Layer。

## 6.1. Layer 基类
`Layer` 是所有层的基类，定义了层的生命周期接口和调试名称。
```cpp
class HAZEL_API Layer {
public:
	explicit Layer(const std::string& debug_name = "Layer");
	virtual ~Layer();

	// 层被添加到应用中时调用
	virtual void onAttach() {}
	// 层被从应用中移除时调用
	virtual void onDetach() {}
	// 每帧更新时调用
	virtual void onUpdate() {}
	// 事件到达该层时调用
	virtual void onEvent(Event& event) {}

	inline const std::string& getName() const { return debug_name_; }

protected:
	std::string debug_name_;
};
```
各接口的作用：
- `onAttach()`：Layer 加入应用时执行初始化逻辑，例如创建资源、注册状态。
- `onDetach()`：Layer 从应用移除时执行清理逻辑。
- `onUpdate()`：主循环每一帧调用，用于更新逻辑或渲染内容。
- `onEvent(Event& event)`：接收并处理事件，Layer 可以通过设置事件的 `handled_` 状态阻止事件继续向下传播。
- `debug_name_`：用于标识 Layer，方便调试。

当前 `Layer.cpp` 中只实现了构造函数和析构函数：
```cpp
Layer::Layer(const std::string& debug_name) : debug_name_(debug_name) {}

Layer::~Layer() {}
```

## 6.2. LayerStack
`LayerStack` 用于管理所有 Layer。内部通过一个 `std::vector<Layer*>` 保存层对象，并用 `layer_insert_` 区分普通 Layer 和 Overlay 的插入位置。
```cpp
class HAZEL_API LayerStack {
public:
	LayerStack();
	~LayerStack();

	void pushLayer(Layer* layer);
	void pushOverlay(Layer* overlay);
	void popLayer(Layer* layer);
	void popOverlay(Layer* overlay);

	std::vector<Layer*>::iterator begin() { return layers_.begin(); }
	std::vector<Layer*>::iterator end() { return layers_.end(); }

private:
	std::vector<Layer*> layers_;
	std::vector<Layer*>::iterator layer_insert_;
};
```

LayerStack 的布局可以理解为：
```text
layers_: [Layer, Layer, Layer, Overlay, Overlay]
                         ^
                   layer_insert_
```
普通 Layer 插入到 `layer_insert_` 位置之前，Overlay 始终追加到数组末尾。因此 Overlay 位于更上层，适合放调试面板、ImGui 界面等需要优先接收事件和后渲染的内容。

### 6.2.1. 添加 Layer
```cpp
void LayerStack::pushLayer(Layer* layer) {
	layer_insert_ = layers_.emplace(layer_insert_, layer);
}
```
普通 Layer 会插入到 Overlay 区域之前。`emplace` 返回新插入元素的位置，并重新赋值给 `layer_insert_`，用于记录下一次普通 Layer 的插入位置。

### 6.2.2. 添加 Overlay
```cpp
void LayerStack::pushOverlay(Layer* overlay) {
	layers_.emplace_back(overlay);
}
```
Overlay 直接追加到 `layers_` 末尾，始终处于最上层。

### 6.2.3. 移除 Layer
```cpp
void LayerStack::popLayer(Layer* layer) {
	auto it = std::ranges::find(layers_, layer);
	if (it != layers_.end()) {
		layers_.erase(it);
		layer_insert_--;
	}
}
```
移除普通 Layer 后，需要同步调整 `layer_insert_`，保持普通 Layer 和 Overlay 的分界位置正确。

### 6.2.4. 移除 Overlay
```cpp
void LayerStack::popOverlay(Layer* overlay) {
	auto it = std::ranges::find(layers_, overlay);
	if (it != layers_.end()) {
		layers_.erase(it);
	}
}
```
Overlay 位于末尾区域，移除时不需要调整普通 Layer 的插入位置。

### 6.2.5. 生命周期管理
`LayerStack` 析构时会释放所有保存的 Layer 指针：
```cpp
LayerStack::~LayerStack() {
	for (Layer* layer : layers_) {
		delete layer;
	}
}
```
因此应用通过 `new` 创建 Layer 后交给 `LayerStack` 管理，退出时由 `LayerStack` 统一销毁。

## 6.3. Application 中的 Layer 管理
`Application` 对外提供两个接口添加层：
```cpp
void Application::pushLayer(Layer* layer) {
	layer_stack_.pushLayer(layer);
	layer->onAttach();
}

void Application::pushOverlay(Layer* overlay) {
	layer_stack_.pushOverlay(overlay);
	overlay->onAttach();
}
```
调用流程为：先把 Layer 放入 `LayerStack`，然后调用该 Layer 的 `onAttach()` 完成初始化。

## 6.4. Layer 的更新流程
在主循环中，`Application::run()` 会按照 `LayerStack` 中的顺序，从前到后调用每个 Layer 的 `onUpdate()`：
```cpp
void Application::run() {
	while (running_) {

		//...
		for (Layer* layer : layer_stack_) {
			layer->onUpdate();
		}

		window_->onUpdate();
	}
}
```
因为 Overlay 追加在末尾，所以更新顺序通常是：先更新普通 Layer，再更新 Overlay。

## 6.5. Layer 的事件分发流程
窗口系统产生事件后，会通过回调进入 `Application::onEvent()`。应用先处理全局事件，例如窗口关闭事件，然后再把事件分发给各个 Layer。
```cpp
void Application::onEvent(Event& e) {
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::onWindowClose));

	// 从后往前遍历，先处理上层的事件
	for (auto it = layer_stack_.end(); it != layer_stack_.begin();) {
		(*--it)->onEvent(e);
		// 如果事件被处理，就跳出循环
		if (e.isHandled()) {
			break;
		}
	}
}
```
事件分发特点：
- 遍历方向是从后往前，即从最上层开始处理。
- Overlay 位于 `LayerStack` 末尾，所以 Overlay 会优先接收事件。
- 如果某一层处理了事件并将事件标记为 `handled`，事件就不会继续传给更底层的 Layer。

该设计适合 UI 场景。例如 ImGui Overlay 在最上层，鼠标或键盘事件可以优先被 ImGui 消费，避免底层游戏逻辑继续响应同一个输入。

## 6.6. Sandbox 中的 Layer 使用示例
Sandbox 定义了一个自定义层 `ExampleLayer`，并在应用构造函数中添加普通 Layer 和 ImGui Overlay。
```cpp
class ExampleLayer : public hazel::Layer {
public:
	ExampleLayer() : Layer("Example") {}

	void onUpdate() override { HZ_INFO("ExampleLayer::Update"); }
	void onEvent(hazel::Event& event) override { HZ_TRACE("{0}", event.toString()); }
};

class Sandbox : public hazel::Application {
public:
	Sandbox() {
		pushLayer(new ExampleLayer());
		pushOverlay(new hazel::ImGuiLayer());
	}
	~Sandbox() = default;
};
```
运行时流程：
```text
Sandbox 构造
  -> pushLayer(new ExampleLayer())
     -> LayerStack::pushLayer()
     -> ExampleLayer::onAttach()
  -> pushOverlay(new ImGuiLayer())
     -> LayerStack::pushOverlay()
     -> ImGuiLayer::onAttach()

Application::run()
  -> ExampleLayer::onUpdate()
  -> ImGuiLayer::onUpdate()
  -> Window::onUpdate()

窗口事件产生
  -> Application::onEvent()
  -> ImGuiLayer::onEvent()
  -> ExampleLayer::onEvent()
```

## 6.7. Layer 层整体流程
```text
┌─────────────────────────────────────────────────────────────┐
│                    创建应用                                  │
├─────────────────────────────────────────────────────────────┤
│  Sandbox()                                                   │
│    pushLayer(ExampleLayer)                                   │
│    pushOverlay(ImGuiLayer)                                   │
└─────────────────────────┬───────────────────────────────────┘
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                    LayerStack 管理                           │
├─────────────────────────────────────────────────────────────┤
│  普通 Layer 插入到 layer_insert_ 前                          │
│  Overlay 追加到 layers_ 末尾                                 │
│  layers_: [普通 Layer..., Overlay...]                        │
└─────────────────────────┬───────────────────────────────────┘
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                    每帧更新                                  │
├─────────────────────────────────────────────────────────────┤
│  Application::run()                                          │
│    for (Layer* layer : layer_stack_)                         │
│        layer->onUpdate()                                     │
│    window_->onUpdate()                                       │
└─────────────────────────┬───────────────────────────────────┘
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                    事件处理                                  │
├─────────────────────────────────────────────────────────────┤
│  Window 回调 -> Application::onEvent()                       │
│  从 LayerStack 尾部向前遍历                                  │
│  Overlay 优先处理事件                                        │
│  event.isHandled() 为 true 时停止继续分发                    │
└─────────────────────────────────────────────────────────────┘
```
## 6.8. ImGui Layer
`ImGuiLayer` 是一个特殊的 Overlay，用于把 Dear ImGui 接入 Hazel 的 Layer 系统。Sandbox 中通过 `pushOverlay(new hazel::ImGuiLayer())` 添加它，因此它位于普通 Layer 之上，可以在更新和事件处理时作为上层 UI 使用。

### 6.8.1. 类定义
```cpp
class HAZEL_API ImGuiLayer : public Layer {
public:
	ImGuiLayer();
	~ImGuiLayer() override = default;

	ImGuiLayer(const ImGuiLayer&) = delete;
	ImGuiLayer& operator=(const ImGuiLayer&) = delete;
	ImGuiLayer(ImGuiLayer&&) = delete;
	ImGuiLayer& operator=(ImGuiLayer&&) = delete;

	void onAttach() override;
	void onDetach() override;
	void onUpdate() override;
	void onEvent(Event& event) override;

private:
	bool onMouseButtonPressedEvent(MouseButtonPressedEvent& e);
	bool onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
	bool onMouseMovedEvent(MouseMovedEvent& e);
	bool onMouseScrolledEvent(MouseScrolledEvent& e);
	bool onKeyPressedEvent(KeyPressedEvent& e);
	bool onKeyReleasedEvent(KeyReleasedEvent& e);
	bool onKeyTypedEvent(KeyTypedEvent& e);
	bool onWindowResizeEvent(WindowResizeEvent& e);

private:
	// 时间戳, 用于帧同步
	float time_ = 0.0F;
};
```
`ImGuiLayer` 继承自 `Layer`，重写了 Layer 的生命周期函数，并额外定义了一组私有事件处理函数，用于把 Hazel 自己的事件系统转换为 ImGui 可识别的输入状态。

拷贝构造、拷贝赋值、移动构造和移动赋值都被删除，表示 `ImGuiLayer` 不应该被复制或移动。它作为 Layer 指针交给 `LayerStack` 管理生命周期。

当前 `onDetach()` 仍为空，其他主要逻辑分别在 `onAttach()`、`onUpdate()` 和 `onEvent()` 中完成。

构造函数中会给 Layer 设置调试名称：
```cpp
ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}
```

### 6.8.2. 初始化流程
`onAttach()` 在 `Application::pushOverlay()` 中被调用，用于初始化 ImGui 的上下文、样式和渲染后端。
```cpp
void ImGuiLayer::onAttach() {
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

	ImGui_ImplOpenGL3_Init("#version 410");
}
```
初始化步骤：
- `ImGui::CreateContext()`：创建 ImGui 全局上下文，后续 `ImGui::GetIO()`、`ImGui::NewFrame()` 等接口都依赖该上下文。
- `ImGui::StyleColorsDark()`：使用 ImGui 默认暗色主题。
- `ImGui::GetIO()`：获取 ImGui 的输入输出配置对象。
- `ImGuiBackendFlags_HasMouseCursors`：标记后端支持鼠标光标切换。
- `ImGuiBackendFlags_HasSetMousePos`：标记后端支持设置鼠标位置。
- `ImGui_ImplOpenGL3_Init("#version 410")`：初始化 OpenGL3 渲染后端，并指定 GLSL 版本。

当前项目只接入了自定义的 OpenGL 渲染后端，没有直接使用 `imgui_impl_glfw`。因此平台输入不是由 ImGui 官方 GLFW backend 自动处理，而是在 `ImGuiLayer::onEvent()` 中手动把 Hazel 事件写入 `ImGuiIO`。

### 6.8.3. 每帧更新流程
`onUpdate()` 每一帧由 `Application::run()` 调用，负责设置 ImGui 帧参数、创建新帧、绘制 UI 并提交渲染数据。
```cpp
void ImGuiLayer::onUpdate() {
	ImGuiIO& io = ImGui::GetIO();
	Application& app = Application::getInstance();
	io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

	float time = static_cast<float>(glfwGetTime());
	io.DeltaTime = time_ > 0.0F ? (time - time_) : (1.0F / 60.0F);
	time_ = time;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	static bool s_show = true;
	ImGui::ShowDemoWindow(&s_show);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
```
每帧执行顺序：
- 获取 `ImGuiIO`，写入当前窗口尺寸到 `io.DisplaySize`。
- 通过 `glfwGetTime()` 计算当前帧和上一帧的时间间隔，写入 `io.DeltaTime`。
- 调用 `ImGui_ImplOpenGL3_NewFrame()` 确保 OpenGL 渲染后端准备好本帧资源。
- 调用 `ImGui::NewFrame()` 开始构建新的 ImGui 帧。
- 当前示例调用 `ImGui::ShowDemoWindow(&s_show)` 显示 ImGui 官方 Demo 窗口。
- 调用 `ImGui::Render()` 生成绘制命令。
- 调用 `ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData())` 将 ImGui 绘制命令提交给 OpenGL。

### 6.8.4. 事件转发流程
`onEvent()` 通过 `EventDispatcher` 识别不同事件类型，并分发到对应的私有处理函数：
```cpp
void ImGuiLayer::onEvent(Event& event) {
	EventDispatcher dispatcher(event);
	dispatcher.dispatch<MouseButtonPressedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::onMouseButtonPressedEvent));
	dispatcher.dispatch<MouseButtonReleasedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::onMouseButtonReleasedEvent));
	dispatcher.dispatch<MouseMovedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::onMouseMovedEvent));
	dispatcher.dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::onMouseScrolledEvent));
	dispatcher.dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::onKeyPressedEvent));
	dispatcher.dispatch<KeyTypedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::onKeyTypedEvent));
	dispatcher.dispatch<KeyReleasedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::onKeyReleasedEvent));
	dispatcher.dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::onWindowResizeEvent));
}
```
事件来源是 `WindowsWindow` 中注册的 GLFW 回调。例如键盘、字符输入、鼠标按钮、滚轮和光标位置都会被转换成 Hazel 事件，然后通过窗口的 `event_callback` 进入 `Application::onEvent()`，最后按 LayerStack 从上到下分发给 `ImGuiLayer`。

这些处理函数当前都返回 `false`，因此 `EventDispatcher` 不会把事件标记为已处理，事件仍会继续传递给下层 Layer。这表示 ImGui 可以同步输入状态，但不会阻止游戏层或业务层继续收到同一个事件。

### 6.8.5. 鼠标事件处理
鼠标按钮事件直接写入 `ImGuiIO::MouseDown`：
```cpp
bool ImGuiLayer::onMouseButtonPressedEvent(MouseButtonPressedEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[e.getMouseButton()] = true;

	return false;
}

bool ImGuiLayer::onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[e.getMouseButton()] = false;

	return false;
}
```
鼠标移动事件更新 `io.MousePos`，滚轮事件累加到 `io.MouseWheelH` 和 `io.MouseWheel`：
```cpp
bool ImGuiLayer::onMouseMovedEvent(MouseMovedEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(e.getX(), e.getY());

	return false;
}

bool ImGuiLayer::onMouseScrolledEvent(MouseScrolledEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += e.getXOffset();
	io.MouseWheel += e.getYOffset();

	return false;
}
```
这样 ImGui 就能知道鼠标位置、鼠标按钮状态和滚轮变化，用于窗口拖拽、按钮点击、滚动区域等 UI 交互。

### 6.8.6. 键盘事件处理
键盘按下和释放事件会先通过 `KeyUtil::gLFWKeyToImGuiKey()` 把 GLFW 按键码转换成 ImGui 的 `ImGuiKey`，再调用 `io.AddKeyEvent()`：
```cpp
bool ImGuiLayer::onKeyPressedEvent(KeyPressedEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	ImGuiKey imgui_key = KeyUtil::gLFWKeyToImGuiKey(e.getKeyCode());

	if (imgui_key != ImGuiKey_None) {
		io.AddKeyEvent(imgui_key, true);
	}

	return false;
}

bool ImGuiLayer::onKeyReleasedEvent(KeyReleasedEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	ImGuiKey imgui_key = KeyUtil::gLFWKeyToImGuiKey(e.getKeyCode());

	if (imgui_key != ImGuiKey_None) {
		io.AddKeyEvent(imgui_key, false);
	}

	return false;
}
```
`KeyUtil` 当前覆盖了常用按键、方向键、编辑键、数字键、小键盘、修饰键、字母键和 F1-F12。无法识别的按键返回 `ImGuiKey_None`，不会提交给 ImGui。

字符输入事件使用 GLFW 的 char callback 产生 `KeyTypedEvent`，用于文本输入：
```cpp
bool ImGuiLayer::onKeyTypedEvent(KeyTypedEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	int keycode = e.getKeyCode();
	if (keycode > 0 && keycode < 0x10000) {
		io.AddInputCharacter(static_cast<uint16_t>(keycode));
	}

	return false;
}
```
`KeyPressedEvent` 负责按键状态，例如快捷键和导航；`KeyTypedEvent` 负责字符输入，例如在文本框中输入文字。

### 6.8.7. 窗口尺寸变化处理
窗口大小变化时，`ImGuiLayer` 会同步更新 ImGui 的显示尺寸，并调整 OpenGL viewport：
```cpp
bool ImGuiLayer::onWindowResizeEvent(WindowResizeEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(e.getWidth(), e.getHeight());
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	glViewport(0, 0, e.getWidth(), e.getHeight());

	return false;
}
```
这保证窗口缩放后，ImGui 的坐标系统和 OpenGL 的渲染区域保持一致。

### 6.8.8. 时间同步
`time_` 保存上一帧时间，用于计算 `io.DeltaTime`：
```cpp
float time = static_cast<float>(glfwGetTime());
io.DeltaTime = time_ > 0.0F ? (time - time_) : (1.0F / 60.0F);
time_ = time;
```
第一次更新时 `time_` 为 `0.0F`，因此使用默认的 `1.0F / 60.0F` 作为帧间隔。之后每帧使用 GLFW 当前时间减去上一帧时间。

`io.DeltaTime` 对 ImGui 很重要，动画、双击判断、输入重复等行为都依赖该值。

### 6.8.9. OpenGL ImGui Renderer
项目中自定义了 `platform/OpenGL/ImGuiOpenGLRenderer.h/.cpp`，提供 ImGui OpenGL3 渲染后端接口：
```cpp
IMGUI_IMPL_API bool ImGui_ImplOpenGL3_Init(const char* glsl_version = nullptr);
IMGUI_IMPL_API void ImGui_ImplOpenGL3_Shutdown();
IMGUI_IMPL_API void ImGui_ImplOpenGL3_NewFrame();
IMGUI_IMPL_API void ImGui_ImplOpenGL3_RenderDrawData(ImDrawData* draw_data);

IMGUI_IMPL_API bool ImGui_ImplOpenGL3_CreateDeviceObjects();
IMGUI_IMPL_API void ImGui_ImplOpenGL3_DestroyDeviceObjects();
```
这些函数承担的职责：
- `ImGui_ImplOpenGL3_Init()`：创建后端数据，保存 GLSL 版本，设置 `io.BackendRendererUserData`、`io.BackendRendererName` 和 renderer backend flags。
- `ImGui_ImplOpenGL3_NewFrame()`：检查后端是否已创建 OpenGL 设备对象，如果尚未创建则调用 `CreateDeviceObjects()`。
- `ImGui_ImplOpenGL3_RenderDrawData()`：把 ImGui 生成的 `ImDrawData` 转换成 OpenGL 绘制调用。
- `ImGui_ImplOpenGL3_CreateDeviceObjects()`：创建 shader、VBO、EBO、字体纹理等 OpenGL 资源。
- `ImGui_ImplOpenGL3_DestroyDeviceObjects()`：销毁字体纹理、buffer 和 shader program。
- `ImGui_ImplOpenGL3_Shutdown()`：释放后端数据，并清理 ImGui IO 中记录的 renderer backend 状态。

### 6.8.10. 渲染后端初始化
`ImGui_ImplOpenGL3_Init()` 会把 OpenGL 渲染后端数据挂到 ImGui IO 上：
```cpp
bool ImGui_ImplOpenGL3_Init(const char* glsl_version) {
	ImGuiIO& io = ImGui::GetIO();
	IMGUI_CHECKVERSION();
	IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");

	ImGui_ImplOpenGL3_Data* bd = IM_NEW(ImGui_ImplOpenGL3_Data)();
	io.BackendRendererUserData = bd;
	io.BackendRendererName = "imgui_impl_opengl3";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	platform_io.DrawCallback_ResetRenderState = ImGui_ImplOpenGL3_DrawCallback_ResetRenderState;

	if (glsl_version != nullptr) {
		IM_ASSERT(static_cast<int>(std::strlen(glsl_version)) + 2 < IM_ARRAYSIZE(bd->glsl_version_string));
		std::snprintf(bd->glsl_version_string, sizeof(bd->glsl_version_string), "%s\n", glsl_version);
	}

	return true;
}
```
关键点：
- `BackendRendererUserData` 保存后端私有数据 `ImGui_ImplOpenGL3_Data`。
- `BackendRendererName` 标识当前 renderer backend 名称。
- `ImGuiBackendFlags_RendererHasVtxOffset` 表示渲染后端支持 `ImDrawCmd::VtxOffset`，可以处理较大的 mesh。
- `glsl_version_string` 会被拼接到 shader 源码前，用于指定 GLSL 版本。

### 6.8.11. OpenGL 资源创建
当 `ImGui_ImplOpenGL3_NewFrame()` 发现 `shader_handle == 0` 时，会调用 `ImGui_ImplOpenGL3_CreateDeviceObjects()` 创建渲染资源。

后端数据结构如下：
```cpp
struct ImGui_ImplOpenGL3_Data {
	char glsl_version_string[32] = "#version 130\n";
	GLuint shader_handle = 0;
	GLint attrib_location_tex = 0;
	GLint attrib_location_proj_mtx = 0;
	GLuint attrib_location_vtx_pos = 0;
	GLuint attrib_location_vtx_uv = 0;
	GLuint attrib_location_vtx_color = 0;
	GLuint vbo_handle = 0;
	GLuint elements_handle = 0;
	GLuint font_texture = 0;
};
```
创建内容包括：
- 编译顶点着色器和片段着色器。
- 链接 shader program。
- 查询 uniform 和 attribute 位置，例如 `Texture`、`ProjMtx`、`Position`、`UV`、`Color`。
- 创建顶点缓冲 `vbo_handle` 和索引缓冲 `elements_handle`。
- 创建 ImGui 字体纹理，并写入 `io.Fonts->SetTexID(...)`。

字体纹理创建流程：
```cpp
io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
glGenTextures(1, &bd->font_texture);
glBindTexture(GL_TEXTURE_2D, bd->font_texture);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
io.Fonts->SetTexID(static_cast<ImTextureID>(bd->font_texture));
```
ImGui 绘制文字时，会通过 `ImDrawCmd::GetTexID()` 取出该纹理并绑定到 OpenGL。

### 6.8.12. ImGui 绘制命令到 OpenGL
`ImGui_ImplOpenGL3_RenderDrawData()` 是真正执行渲染的函数。它会遍历 ImGui 生成的绘制列表和绘制命令，并转换成 OpenGL 调用。

主要流程：
```text
ImGui::Render()
  -> 生成 ImDrawData
  -> ImGui_ImplOpenGL3_RenderDrawData(draw_data)
     -> 备份当前 OpenGL 状态
     -> 设置 ImGui 渲染状态
     -> 遍历 draw_data->CmdLists
        -> 上传顶点缓冲和索引缓冲
        -> 遍历每个 ImDrawCmd
           -> 设置裁剪区域 glScissor
           -> 绑定纹理 glBindTexture
           -> 绘制元素 glDrawElementsBaseVertex
     -> 恢复之前的 OpenGL 状态
```
设置渲染状态时，会启用 Alpha 混合，关闭深度测试和剔除，并开启裁剪测试：
```cpp
glEnable(GL_BLEND);
glBlendEquation(GL_FUNC_ADD);
glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
glDisable(GL_CULL_FACE);
glDisable(GL_DEPTH_TEST);
glDisable(GL_STENCIL_TEST);
glEnable(GL_SCISSOR_TEST);
```
这些设置符合 UI 渲染需求：UI 通常需要透明混合，不参与 3D 深度测试，并且每个窗口/控件需要裁剪到自己的矩形区域。

绘制每条命令时，会根据 ImGui 提供的裁剪矩形设置 `glScissor()`，绑定对应纹理，然后调用 `glDrawElementsBaseVertex()`：
```cpp
glScissor(static_cast<int>(clip_min.x), static_cast<int>(static_cast<float>(fb_height) - clip_max.y),
          static_cast<int>(clip_max.x - clip_min.x), static_cast<int>(clip_max.y - clip_min.y));
glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(pcmd->GetTexID()));
glDrawElementsBaseVertex(GL_TRIANGLES, static_cast<GLsizei>(pcmd->ElemCount),
                         sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
                         reinterpret_cast<void*>(static_cast<intptr_t>(pcmd->IdxOffset * sizeof(ImDrawIdx))),
                         static_cast<GLint>(pcmd->VtxOffset));
```

### 6.8.13. 当前实现的限制
当前 `ImGuiLayer` 已经完成了 ImGui 的显示、OpenGL 渲染和基础输入事件转发，但还存在一些未完善的点：
- `onDetach()` 为空，没有调用 `ImGui_ImplOpenGL3_Shutdown()` 和 `ImGui::DestroyContext()` 清理 ImGui 资源。
- 项目没有接入完整 platform backend，例如 `imgui_impl_glfw`，因此鼠标光标形状、剪贴板、手柄、IME 等平台能力还没有完整实现。
- 事件处理函数当前都返回 `false`，即使 ImGui 想捕获鼠标或键盘，事件仍会继续传给下层 Layer。后续可以根据 `ImGuiIO::WantCaptureMouse` 和 `ImGuiIO::WantCaptureKeyboard` 决定是否标记事件为 handled。
- 当前 UI 内容固定为 `ImGui::ShowDemoWindow(&s_show)`，后续可以把业务 UI 拆成独立函数或独立 Layer。

## 6.9. ImGui Layer 整体流程
```text
┌─────────────────────────────────────────────────────────────┐
│                    Sandbox 创建                              │
├─────────────────────────────────────────────────────────────┤
│  pushOverlay(new ImGuiLayer())                               │
│  LayerStack 将 ImGuiLayer 放到最上层                         │
└─────────────────────────┬───────────────────────────────────┘
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                    onAttach 初始化                           │
├─────────────────────────────────────────────────────────────┤
│  ImGui::CreateContext()                                      │
│  ImGui::StyleColorsDark()                                    │
│  配置 ImGuiIO BackendFlags                                   │
│  ImGui_ImplOpenGL3_Init("#version 410")                     │
└─────────────────────────┬───────────────────────────────────┘
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                    输入事件                                  │
├─────────────────────────────────────────────────────────────┤
│  GLFW callbacks -> Hazel Event                              │
│  Application::onEvent()                                      │
│  ImGuiLayer::onEvent()                                       │
│  写入 ImGuiIO: MouseDown / MousePos / MouseWheel / KeyEvent │
└─────────────────────────┬───────────────────────────────────┘
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                    每帧 onUpdate                             │
├─────────────────────────────────────────────────────────────┤
│  设置 io.DisplaySize 和 io.DeltaTime                         │
│  ImGui_ImplOpenGL3_NewFrame()                                │
│  ImGui::NewFrame()                                           │
│  构建 UI: ImGui::ShowDemoWindow()                            │
│  ImGui::Render()                                             │
└─────────────────────────┬───────────────────────────────────┘
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                    OpenGL 渲染后端                            │
├─────────────────────────────────────────────────────────────┤
│  ImGui_ImplOpenGL3_RenderDrawData()                          │
│  上传顶点/索引数据                                            │
│  设置裁剪矩形和纹理                                           │
│  调用 glDrawElementsBaseVertex() 绘制 UI                     │
│  恢复原 OpenGL 状态                                           │
└─────────────────────────────────────────────────────────────┘
```

