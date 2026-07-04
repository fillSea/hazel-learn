#pragma once

#include <memory>

#include "hazel/Core.h"
#include "hazel/renderer/Buffer.h"

namespace hazel {

/**
 * @brief 顶点数组抽象接口（VAO）
 *
 * 将一个或多个顶点缓冲（VBO）与一个索引缓冲（EBO/IBO）绑定在一起，
 * 形成完整的绘制单元。具体实现由各图形 API 子类提供（如 OpenGLVertexArray）。
 * 不可拷贝、不可移动。
 */
class HAZEL_API VertexArray {
public:
	VertexArray() = default;
	virtual ~VertexArray() = default;

	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(const VertexArray&) = delete;
	VertexArray(VertexArray&&) = delete;
	VertexArray& operator=(VertexArray&&) = delete;

	/**
	 * @brief 工厂方法：根据当前图形 API 创建对应的 VertexArray 实例
	 * @return 新创建的 VertexArray 指针
	 */
	static VertexArray* create();

	/**
	 * @brief 绑定当前顶点数组到图形管线
	 */
	virtual void bind() const = 0;

	/**
	 * @brief 解绑当前顶点数组
	 */
	virtual void unBind() const = 0;

	/**
	 * @brief 添加顶点缓冲到顶点数组
	 * @param vertex_buffer 要添加的顶点缓冲对象
	 */
	virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertex_buffer) = 0;

	/**
	 * @brief 设置索引缓冲（替换已有的索引缓冲）
	 * @param index_buffer 要设置的索引缓冲对象
	 */
	virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& index_buffer) = 0;

	/**
	 * @brief 获取所有顶点缓冲
	 * @return 顶点缓冲列表的常引用
	 */
	[[nodiscard]] virtual const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const = 0;

	/**
	 * @brief 获取索引缓冲
	 * @return 索引缓冲的常引用
	 */
	[[nodiscard]] virtual const std::shared_ptr<IndexBuffer>& getIndexBuffer() const = 0;
};

}  // namespace hazel