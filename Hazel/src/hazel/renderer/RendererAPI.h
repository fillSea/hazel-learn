#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace hazel {

/**
 * @brief 渲染 API 抽象接口
 *
 * 定义与平台无关的渲染操作纯虚接口，具体实现由各图形 API 子类提供
 * （如 OpenGLRendererAPI）。RenderCommand 通过此接口完成静态分发。
 */
class RendererAPI {
public:
	/**
	 * @brief 支持的图形 API 枚举
	 */
	enum class API { None = 0, OpenGL = 1 };

public:
	/**
	 * @brief 设置清屏颜色
	 * @param color RGBA 颜色向量，取值范围 [0, 1]
	 */
	virtual void setClearColor(const glm::vec4& color) = 0;

	/**
	 * @brief 清除当前帧缓冲（颜色缓冲与深度缓冲）
	 */
	virtual void clear() = 0;

	/**
	 * @brief 按索引缓冲绘制顶点数组
	 * @param vertex_array 包含顶点缓冲与索引缓冲的顶点数组对象
	 */
	virtual void drawIndexed(const std::shared_ptr<VertexArray>& vertex_array) = 0;

	/**
	 * @brief 获取当前正在使用的图形 API 类型
	 * @return 当前 API 枚举值
	 */
	inline static API getAPI() { return s_api_; }

private:
	/// 当前激活的图形 API 类型，由 Renderer 在初始化时设置
	static API s_api_;
};

}  // namespace hazel