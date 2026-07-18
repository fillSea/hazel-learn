#include "OpenGLShader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace hazel {

OpenGLShader::OpenGLShader(const std::string& vertex_src, const std::string& fragment_src)
    : Shader(vertex_src, fragment_src) {
	// Create an empty vertex shader handle
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

	// Send the vertex shader source code to GL
	// Note that std::string's .c_str is NULL character terminated.
	const GLchar* source = vertex_src.c_str();
	glShaderSource(vertex_shader, 1, &source, 0);

	// Compile the vertex shader
	glCompileShader(vertex_shader);

	GLint is_compiled = 0;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_compiled);
	if (is_compiled == GL_FALSE) {
		GLint max_length = 0;
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &max_length);

		// The maxLength includes the NULL character
		std::vector<GLchar> info_log(max_length);
		glGetShaderInfoLog(vertex_shader, max_length, &max_length, &info_log[0]);

		// We don't need the shader anymore.
		glDeleteShader(vertex_shader);

		HZ_CORE_ERROR("{0}", info_log.data());
		HZ_CORE_ASSERT(false, "Vertex shader compilation failure!");
		return;
	}

	// Create an empty fragment shader handle
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	// Send the fragment shader source code to GL
	// Note that std::string's .c_str is NULL character terminated.
	source = fragment_src.c_str();
	glShaderSource(fragment_shader, 1, &source, 0);

	// Compile the fragment shader
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_compiled);
	if (is_compiled == GL_FALSE) {
		GLint max_length = 0;
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &max_length);

		// The maxLength includes the NULL character
		std::vector<GLchar> info_log(max_length);
		glGetShaderInfoLog(fragment_shader, max_length, &max_length, &info_log[0]);

		// We don't need the shader anymore.
		glDeleteShader(fragment_shader);
		// Either of them. Don't leak shaders.
		glDeleteShader(vertex_shader);

		HZ_CORE_ERROR("{0}", info_log.data());
		HZ_CORE_ASSERT(false, "Fragment shader compilation failure!");
		return;
	}

	// Vertex and fragment shaders are successfully compiled.
	// Now time to link them together into a program.
	// Get a program object.
	renderer_id_ = glCreateProgram();
	GLuint program = renderer_id_;

	// Attach our shaders to our program
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	// Link our program
	glLinkProgram(program);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint is_linked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, static_cast<int*>(&is_linked));
	if (is_linked == GL_FALSE) {
		GLint max_length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

		// The maxLength includes the NULL character
		std::vector<GLchar> info_log(max_length);
		glGetProgramInfoLog(program, max_length, &max_length, info_log.data());

		// We don't need the program anymore.
		glDeleteProgram(program);
		// Don't leak shaders either.
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		HZ_CORE_ERROR("{0}", info_log.data());
		HZ_CORE_ASSERT(false, "Shader link failure!");
		return;
	}

	// Always detach shaders after a successful link.
	glDetachShader(program, vertex_shader);
	glDetachShader(program, fragment_shader);
}

OpenGLShader::~OpenGLShader() {
	glDeleteProgram(renderer_id_);
}

void OpenGLShader::bind() const {
	glUseProgram(renderer_id_);
}

void OpenGLShader::unBind() const {
	glUseProgram(0);
}

void OpenGLShader::uploadUniformInt(const std::string& name, int value) const {
	GLint location = glGetUniformLocation(renderer_id_, name.c_str());
	glUniform1i(location, value);
}

void OpenGLShader::uploadUniformFloat(const std::string& name, float value) const {
	GLint location = glGetUniformLocation(renderer_id_, name.c_str());
	glUniform1f(location, value);
}

void OpenGLShader::uploadUniformFloat2(const std::string& name, const glm::vec2& value) const {
	GLint location = glGetUniformLocation(renderer_id_, name.c_str());
	glUniform2f(location, value.x, value.y);
}

void OpenGLShader::uploadUniformFloat3(const std::string& name, const glm::vec3& value) const {
	GLint location = glGetUniformLocation(renderer_id_, name.c_str());
	glUniform3f(location, value.x, value.y, value.z);
}

void OpenGLShader::uploadUniformFloat4(const std::string& name, const glm::vec4& value) const {
	GLint location = glGetUniformLocation(renderer_id_, name.c_str());
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGLShader::uploadUniformMat3(const std::string& name, const glm::mat3& matrix) const {
	GLint location = glGetUniformLocation(renderer_id_, name.c_str());
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::uploadUniformMat4(const std::string& name, const glm::mat4& matrix) const {
	GLint location = glGetUniformLocation(renderer_id_, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

}  // namespace hazel