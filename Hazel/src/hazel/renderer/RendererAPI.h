#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"
#include "hazel/core/Core.h"

namespace hazel {

/**
 * @brief 渲染 API 抽象接口
 *
 * 定义与平台无关的渲染操作纯虚接口，具体实现由各图形 API 子类提供
 * （如 OpenGLRendererAPI）。RenderCommand 通过此接口完成静态分发。
 */
class HAZEL_API RendererAPI {
public:
	/**
	 * @brief 支持的图形 API 枚举
	 */
	enum class API { None = 0, OpenGL = 1 };

public:
	virtual void init() = 0;

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
	 * @param indexCount 要绘制的索引数量，0 表示绘制所有索引
	 */
	virtual void drawIndexed(const std::shared_ptr<VertexArray>& vertex_array, uint32_t index_count = 0) = 0;

	/**
	 * @brief 获取当前正在使用的图形 API 类型
	 * @return 当前 API 枚举值
	 */
	inline static API getAPI() { return s_api_; }

	/**
	 * @brief 设置视口区域
	 *
	 * @param x 视口左上角 x 坐标
	 * @param y 视口左上角 y 坐标
	 * @param width 视口宽度
	 * @param height 视口高度
	 */
	virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

private:
	/// 当前激活的图形 API 类型，由 Renderer 在初始化时设置
	static API s_api_;
};

}  // namespace hazel