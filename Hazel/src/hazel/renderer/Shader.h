#pragma once

#include <glm/glm.hpp>
#include <string>

#include "hazel/core/Core.h"

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

	static Ref<Shader> create(const std::string& filepath);
	static Ref<Shader> create(const std::string& name, const std::string& vertex_src, const std::string& fragment_src);

	[[nodiscard]] virtual const std::string& getName() const = 0;

	virtual void setInt(const std::string& name, int value) = 0;
	virtual void setIntArray(const std::string& name, int* values, uint32_t count) = 0;
	virtual void setFloat(const std::string& name, float value) = 0;
	virtual void setFloat3(const std::string& name, const glm::vec3& value) = 0;
	virtual void setFloat4(const std::string& name, const glm::vec4& value) = 0;
	virtual void setMat4(const std::string& name, const glm::mat4& value) = 0;
};

/**
 * @brief 着色器库
 *
 * 管理着色器程序的注册、加载与获取，通过名称索引缓存已创建的着色器实例。
 * 支持从文件加载着色器并自动注册到库中，避免重复创建。
 */
class HAZEL_API ShaderLibrary {
public:
	/**
	 * @brief 以指定名称注册着色器
	 * @param name 着色器名称
	 * @param shader 着色器实例
	 */
	void add(const std::string& name, const Ref<Shader>& shader);
	/**
	 * @brief 以着色器自身名称注册着色器
	 * @param shader 着色器实例
	 */
	void add(const Ref<Shader>& shader);

	/**
	 * @brief 从文件加载着色器并注册
	 * @param filepath 着色器文件路径
	 * @return 加载的着色器实例
	 */
	Ref<Shader> load(const std::string& filepath);
	/**
	 * @brief 从文件加载着色器并以指定名称注册
	 * @param name 着色器名称
	 * @param filepath 着色器文件路径
	 * @return 加载的着色器实例
	 */
	Ref<Shader> load(const std::string& name, const std::string& filepath);

	/**
	 * @brief 根据名称获取已注册的着色器
	 * @param name 着色器名称
	 * @return 对应的着色器实例
	 */
	Ref<Shader> get(const std::string& name);

	/**
	 * @brief 检查指定名称的着色器是否已注册
	 * @param name 着色器名称
	 * @return 存在返回 true，否则返回 false
	 */
	bool isExist(const std::string& name) const;

private:
	std::unordered_map<std::string, Ref<Shader>> shaders_;  ///< 着色器映射表，名称 -> 着色器实例
};

}  // namespace hazel