#pragma once

#include <glm/glm.hpp>
#include <string>

#include "hazel/Core.h"

namespace hazel {

/**
 * @brief 着色器程序
 *
 * 封装顶点着色器与片段着色器的编译、链接及使用。
 * 不可拷贝、不可移动。
 */
class HAZEL_API Shader {
public:
	/**
	 * @brief 构造着色器程序
	 * @param vertex_src 顶点着色器源码
	 * @param fragment_src 片段着色器源码
	 */
	Shader(const std::string& vertex_src, const std::string& fragment_src);
	/**
	 * @brief 析构着色器程序，释放 GPU 资源
	 */
	virtual ~Shader() = default;

	Shader(const Shader& other) = delete;
	Shader& operator=(const Shader& other) = delete;
	Shader(Shader&& other) = delete;
	Shader& operator=(Shader&& other) = delete;

	virtual void bind() const = 0;
	virtual void unBind() const = 0;

	static Shader* create(const std::string& vertex_src, const std::string& fragment_src);
};

}  // namespace hazel