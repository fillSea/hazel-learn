#pragma once

#include "hazel/Core.h"
#include "hazel/events/Event.h"

namespace hazel {

/**
 * @brief 层基类
 *
 */
class HAZEL_API Layer {
public:
	explicit Layer(const std::string& debug_name = "Layer");
	virtual ~Layer();

	// 事件回调
	// 层被添加到应用中时调用
	virtual void onAttach() {}
	// 层被从应用中移除时调用
	virtual void onDetach() {}
	// 层更新时调用
	virtual void onUpdate() {}
	// 层事件处理时调用
	virtual void onEvent(Event& event) {}
	// ImGui渲染时调用
	virtual void onImGuiRender() {}

	inline const std::string& getName() const { return debug_name_; }

protected:
	std::string debug_name_;
};

}  // namespace hazel