#pragma once

#include <glm/glm.hpp>

#include "hazel/renderer/Shader.h"

namespace hazel {

class HAZEL_API OpenGLShader : public Shader {
public:
	OpenGLShader(const std::string& vertex_src, const std::string& fragment_src);
	~OpenGLShader() override;

	OpenGLShader(const OpenGLShader& other) = delete;
	OpenGLShader& operator=(const OpenGLShader& other) = delete;
	OpenGLShader(OpenGLShader&& other) = delete;
	OpenGLShader& operator=(OpenGLShader&& other) = delete;

	void bind() const override;
	void unBind() const override;

	void uploadUniformInt(const std::string& name, int value) const;

	void uploadUniformFloat(const std::string& name, float value) const;
	void uploadUniformFloat2(const std::string& name, const glm::vec2& value) const;
	void uploadUniformFloat3(const std::string& name, const glm::vec3& value) const;
	void uploadUniformFloat4(const std::string& name, const glm::vec4& value) const;

	void uploadUniformMat3(const std::string& name, const glm::mat3& matrix) const;
	void uploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;

private:
	uint32_t renderer_id_;
};

}  // namespace hazel