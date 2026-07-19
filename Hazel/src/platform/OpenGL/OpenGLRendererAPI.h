#pragma once

#include "hazel/renderer/RendererAPI.h"

namespace hazel {

/**
 * @brief OpenGL 渲染 API 实现
 *
 * 继承自 RendererAPI 抽象接口，基于 OpenGL 提供具体的渲染操作实现。
 */
class OpenGLRendererAPI : public RendererAPI {
public:
	void init() override;

	/**
	 * @brief 设置 OpenGL 清屏颜色
	 * @param color RGBA 颜色向量，取值范围 [0, 1]
	 */
	void setClearColor(const glm::vec4& color) override;

	/**
	 * @brief 清除 OpenGL 颜色缓冲与深度缓冲
	 */
	void clear() override;

	/**
	 * @brief 使用 OpenGL 按索引缓冲绘制顶点数组
	 * @param vertex_array 包含顶点缓冲与索引缓冲的顶点数组对象
	 */
	void drawIndexed(const std::shared_ptr<VertexArray>& vertex_array) override;
};

}  // namespace hazel