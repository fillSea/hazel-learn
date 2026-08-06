#pragma once

#include <glm/glm.hpp>

#include "hazel/renderer/Shader.h"

// TODO: REMOVE!
typedef unsigned int GLenum;

namespace hazel {

class HAZEL_API OpenGLShader : public Shader {
public:
	explicit OpenGLShader(const std::string& filepath);
	OpenGLShader(const std::string& name, const std::string& vertex_src, const std::string& fragment_src);
	~OpenGLShader() override;

	OpenGLShader(const OpenGLShader& other) = delete;
	OpenGLShader& operator=(const OpenGLShader& other) = delete;
	OpenGLShader(OpenGLShader&& other) = delete;
	OpenGLShader& operator=(OpenGLShader&& other) = delete;

	void bind() const override;
	void unBind() const override;

	[[nodiscard]] const std::string& getName() const override { return name_; }

	void setInt(const std::string& name, int value) override;
	void setFloat(const std::string& name, float value) override;
	void setFloat3(const std::string& name, const glm::vec3& value) override;
	void setFloat4(const std::string& name, const glm::vec4& value) override;
	void setMat4(const std::string& name, const glm::mat4& value) override;

	void uploadUniformInt(const std::string& name, int value) const;

	void uploadUniformFloat(const std::string& name, float value) const;
	void uploadUniformFloat2(const std::string& name, const glm::vec2& value) const;
	void uploadUniformFloat3(const std::string& name, const glm::vec3& value) const;
	void uploadUniformFloat4(const std::string& name, const glm::vec4& value) const;

	void uploadUniformMat3(const std::string& name, const glm::mat3& matrix) const;
	void uploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;

private:
	std::string readFile(const std::string& filepath);
	std::unordered_map<GLenum, std::string> preProcess(const std::string& source);
	void compile(const std::unordered_map<GLenum, std::string>& shader_sources);

private:
	uint32_t renderer_id_;
	std::string name_;
};

}  // namespace hazel