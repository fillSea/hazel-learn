#pragma once

#include <string>

#include "hazel/core/Core.h"

namespace hazel {

/**
 * @brief 纹理基类，定义纹理资源的通用接口
 *
 * 不可拷贝、不可移动，作为所有纹理类型的抽象基类
 */
class HAZEL_API Texture {
public:
	Texture() = default;
	virtual ~Texture() = default;

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&&) = delete;
	Texture& operator=(Texture&&) = delete;

	/// @brief 获取纹理宽度（像素）
	virtual uint32_t getWidth() const = 0;
	/// @brief 获取纹理高度（像素）
	virtual uint32_t getHeight() const = 0;

	/// @brief 将纹理绑定到指定纹理槽
	/// @param slot 纹理槽索引，默认为 0
	virtual void bind(uint32_t slot = 0) const = 0;
};

/**
 * @brief 2D 纹理类，继承自 Texture 基类
 */
class HAZEL_API Texture2D : public Texture {
public:
	/// @brief 从文件路径创建 2D 纹理
	/// @param path 图片文件路径
	/// @return Texture2D 的智能指针
	static Ref<Texture2D> create(const std::string& path);
};

}  // namespace hazel