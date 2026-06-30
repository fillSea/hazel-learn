#pragma once

#include <string>

namespace hazel {

/**
 * @brief 着色器程序
 *
 * 封装顶点着色器与片段着色器的编译、链接及使用。
 * 不可拷贝、不可移动。
 */
class Shader {
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
	~Shader();

	Shader(const Shader& other) = delete;
	Shader& operator=(const Shader& other) = delete;
	Shader(Shader&& other) = delete;
	Shader& operator=(Shader&& other) = delete;

	/**
	 * @brief 绑定当前着色器程序到渲染管线
	 */
	void bind() const;

	/**
	 * @brief 解绑当前着色器程序
	 */
	void unbind() const;

private:
	/// 着色器程序在 GPU 端的标识符（OpenGL shader program ID）
	uint32_t renderer_id_;
};

}  // namespace hazel