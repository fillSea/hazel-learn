#include <utility>

#pragma once

#include "hazel/Core.h"

namespace hazel {

enum class ShaderDataType { None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool };

static uint32_t getShaderDataTypeSize(ShaderDataType type) {
	switch (type) {
		case ShaderDataType::Float:
			return 4;
		case ShaderDataType::Float2:
			return 4 * 2;
		case ShaderDataType::Float3:
			return 4 * 3;
		case ShaderDataType::Float4:
			return 4 * 4;
		case ShaderDataType::Mat3:
			return 4 * 3 * 3;
		case ShaderDataType::Mat4:
			return 4 * 4 * 4;
		case ShaderDataType::Int:
			return 4;
		case ShaderDataType::Int2:
			return 4 * 2;
		case ShaderDataType::Int3:
			return 4 * 3;
		case ShaderDataType::Int4:
			return 4 * 4;
		case ShaderDataType::Bool:
			return 1;
		case ShaderDataType::None:
			break;
	}

	HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
	return 0;
}

/**
 * @brief 缓冲区元素
 * @details 缓冲区元素描述了缓冲区中的数据类型、偏移量、组件数量等信息。
 *
 */
struct BufferElement {
	std::string name;     // 缓冲区元素的名称
	ShaderDataType type;  // 缓冲区元素的数据类型
	uint32_t size;        // 缓冲区元素的大小
	uint32_t offset;      // 缓冲区元素的偏移量
	bool normalized;      // 是否归一化数据

	BufferElement() = default;

	BufferElement(ShaderDataType type, std::string name, bool normalized = false)
	    : name(std::move(name)), type(type), size(getShaderDataTypeSize(type)), offset(0), normalized(normalized) {}

	/**
	 * @brief 获取缓冲区元素的组件数量
	 *
	 * @return uint32_t 缓冲区元素的组件数量
	 */
	[[nodiscard]] uint32_t getComponentCount() const {
		switch (type) {
			case ShaderDataType::Float:
				return 1;
			case ShaderDataType::Float2:
				return 2;
			case ShaderDataType::Float3:
				return 3;
			case ShaderDataType::Float4:
				return 4;
			case ShaderDataType::Mat3:
				return 3 * 3;
			case ShaderDataType::Mat4:
				return 4 * 4;
			case ShaderDataType::Int:
				return 1;
			case ShaderDataType::Int2:
				return 2;
			case ShaderDataType::Int3:
				return 3;
			case ShaderDataType::Int4:
				return 4;
			case ShaderDataType::Bool:
				return 1;
			case ShaderDataType::None:
				break;
		}

		HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}
};

/**
 * @brief 缓冲区布局
 *
 * 描述顶点缓冲中各元素的排列方式，包括每个元素的偏移量与整体步长（stride），
 * 用于在渲染时正确解析和访问顶点属性数据。
 */
class HAZEL_API BufferLayout {
public:
	BufferLayout() = default;

	/**
	 * @brief 通过初始化列表构造缓冲区布局，并自动计算偏移量与步长
	 * @param elements 缓冲区元素初始化列表
	 */
	BufferLayout(const std::initializer_list<BufferElement>& elements) : elements_(elements) {
		calculateOffsetsAndStride();
	}

	/**
	 * @brief 获取步长（单个顶点数据的总字节数）
	 * @return 步长字节数
	 */
	inline uint32_t getStride() const { return stride_; }

	/**
	 * @brief 获取所有缓冲区元素
	 * @return 缓冲区元素列表的常引用
	 */
	inline const std::vector<BufferElement>& getElements() const { return elements_; }

	/// 迭代器支持，用于范围遍历缓冲区元素
	std::vector<BufferElement>::iterator begin() { return elements_.begin(); }
	std::vector<BufferElement>::iterator end() { return elements_.end(); }
	std::vector<BufferElement>::const_iterator begin() const { return elements_.begin(); }
	std::vector<BufferElement>::const_iterator end() const { return elements_.end(); }

private:
	/**
	 * @brief 计算缓冲区元素的偏移量和缓冲区的总偏移量
	 * @details 计算缓冲区元素的偏移量和缓冲区的总偏移量，用于在渲染时正确访问数据。
	 */
	void calculateOffsetsAndStride() {
		uint32_t offset = 0;
		stride_ = 0;
		for (auto& element : elements_) {
			element.offset = offset;
			offset += element.size;
			stride_ += element.size;
		}
	}

private:
	// 缓冲区元素
	std::vector<BufferElement> elements_;
	// 缓冲区的总偏移量
	uint32_t stride_ = 0;
};

/**
 * @brief 顶点缓冲抽象接口（VBO）
 *
 * 存储顶点属性数据（位置、法线、纹理坐标等），
 * 具体实现由各图形 API 子类提供（如 OpenGLVertexBuffer）。
 * 不可拷贝、不可移动。
 */
class HAZEL_API VertexBuffer {
public:
	VertexBuffer() = default;
	virtual ~VertexBuffer() = default;

	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;
	VertexBuffer(VertexBuffer&&) = delete;
	VertexBuffer& operator=(VertexBuffer&&) = delete;

	/**
	 * @brief 工厂方法：根据当前图形 API 创建对应的 VertexBuffer 实例
	 * @param vertices 顶点数据数组
	 * @param size 数据总字节数
	 * @return 新创建的 VertexBuffer 指针
	 */
	static VertexBuffer* create(float* vertices, uint32_t size);

	/**
	 * @brief 绑定当前顶点缓冲到图形管线
	 */
	virtual void bind() const = 0;

	/**
	 * @brief 解绑当前顶点缓冲
	 */
	virtual void unBind() const = 0;

	/**
	 * @brief 获取顶点缓冲的布局描述
	 * @return 缓冲区布局的常引用
	 */
	[[nodiscard]] virtual const BufferLayout& getLayout() const = 0;

	/**
	 * @brief 设置顶点缓冲的布局描述
	 * @param layout 缓冲区布局
	 */
	virtual void setLayout(const BufferLayout& layout) = 0;
};

/**
 * @brief 索引缓冲抽象接口（EBO/IBO）
 *
 * 存储顶点索引数据，用于索引绘制（drawIndexed），
 * 具体实现由各图形 API 子类提供（如 OpenGLIndexBuffer）。
 * 不可拷贝、不可移动。
 */
class HAZEL_API IndexBuffer {
public:
	IndexBuffer() = default;
	virtual ~IndexBuffer() = default;

	IndexBuffer(const IndexBuffer&) = delete;
	IndexBuffer& operator=(const IndexBuffer&) = delete;
	IndexBuffer(IndexBuffer&&) = delete;
	IndexBuffer& operator=(IndexBuffer&&) = delete;

	/**
	 * @brief 工厂方法：根据当前图形 API 创建对应的 IndexBuffer 实例
	 * @param indices 索引数据数组
	 * @param size 数据总字节数
	 * @return 新创建的 IndexBuffer 指针
	 */
	static IndexBuffer* create(uint32_t* indices, uint32_t size);

	/**
	 * @brief 绑定当前索引缓冲到图形管线
	 */
	virtual void bind() const = 0;

	/**
	 * @brief 解绑当前索引缓冲
	 */
	virtual void unBind() const = 0;

	/**
	 * @brief 获取索引数量
	 * @return 索引计数
	 */
	[[nodiscard]] virtual uint32_t getCount() const = 0;
};

}  // namespace hazel