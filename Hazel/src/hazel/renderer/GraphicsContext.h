#pragma once

namespace hazel {

/**
 * @brief 图形上下文
 *
 */
class GraphicsContext {
public:
	// 初始化上下文
	virtual void init() = 0;
	// 交换缓冲区
	virtual void swapBuffers() = 0;
};

}  // namespace hazel