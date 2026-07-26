#pragma once

#include "RendererAPI.h"
#include "hazel/core/Core.h"

namespace hazel {

/**
 * @brief 渲染命令静态分发器
 *
 * 提供与平台无关的渲染命令入口，所有调用均委托给当前平台的 RendererAPI 实现。
 * 遵循 Command 模式，上层只需通过本类发起渲染指令，无需关心底层图形 API 差异。
 */
class HAZEL_API RenderCommand {
public:
	inline static void init() { s_renderer_api_->init(); }

	/**
	 * @brief 设置清屏颜色
	 * @param color RGBA 颜色向量，取值范围 [0, 1]
	 */
	inline static void setClearColor(const glm::vec4& color) { s_renderer_api_->setClearColor(color); }

	/**
	 * @brief 清除当前帧缓冲（颜色缓冲与深度缓冲）
	 */
	inline static void clear() { s_renderer_api_->clear(); }

	/**
	 * @brief 按索引绘制顶点数组
	 * @param vertex_array 包含顶点缓冲与索引缓冲的顶点数组对象
	 */
	inline static void drawIndexed(const std::shared_ptr<VertexArray>& vertex_array) {
		s_renderer_api_->drawIndexed(vertex_array);
	}

	inline static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		s_renderer_api_->setViewport(x, y, width, height);
	}

private:
	/// 当前平台渲染 API 实例指针，由 Renderer 在初始化时设置
	static RendererAPI* s_renderer_api_;
};

}  // namespace hazel