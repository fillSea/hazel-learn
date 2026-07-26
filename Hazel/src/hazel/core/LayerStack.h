#pragma once

#include <vector>

#include "hazel/core/Core.h"
#include "hazel/core/Layer.h"

namespace hazel {
/**
 * @brief 层栈类
 * 用于管理应用中的层，如窗口、渲染器等。
 *
 */
class HAZEL_API LayerStack {
public:
	LayerStack();
	~LayerStack();

	LayerStack(const LayerStack&) = delete;
	LayerStack& operator=(const LayerStack&) = delete;
	LayerStack(LayerStack&&) = delete;
	LayerStack& operator=(LayerStack&&) = delete;

	// 添加层
	void pushLayer(Layer* layer);
	// 添加覆盖层
	void pushOverlay(Layer* overlay);
	// 移除层
	void popLayer(Layer* layer);
	// 移除覆盖层
	void popOverlay(Layer* overlay);

	std::vector<Layer*>::iterator begin() { return layers_.begin(); }
	std::vector<Layer*>::iterator end() { return layers_.end(); }

private:
	// 层栈
	std::vector<Layer*> layers_;
	// 层插入位置
	unsigned int layer_insert_index_{0};
};
}  // namespace hazel