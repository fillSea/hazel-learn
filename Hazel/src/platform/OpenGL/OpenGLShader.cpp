#include "OpenGLShader.h"

#include <glad/glad.h>

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

namespace hazel {
static GLenum shaderTypeFromString(const std::string& type) {
	if (type == "vertex") {
		return GL_VERTEX_SHADER;
	}
	if (type == "fragment" || type == "pixel") {
		return GL_FRAGMENT_SHADER;
	}
	HZ_CORE_ASSERT(false, "Unknown shader type!");
	return 0;
}

OpenGLShader::OpenGLShader(const std::string& filepath) : Shader("", "") {
	std::string source = readFile(filepath);
	auto shader_sources = preProcess(source);
	compile(shader_sources);

	// Extract name from filepath
	auto last_slash = filepath.find_last_of("/\\");
	last_slash = last_slash == std::string::npos ? 0 : last_slash + 1;
	auto last_dot = filepath.rfind('.');
	auto count = last_dot == std::string::npos ? filepath.size() - last_slash : last_dot - last_slash;
	name_ = filepath.substr(last_slash, count);
}

OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertex_src, const std::string& fragment_src)
    : Shader(vertex_src, fragment_src), name_(name) {
	std::unordered_map<GLenum, std::string> sources;
	sources[GL_VERTEX_SHADER] = vertex_src;
	sources[GL_FRAGMENT_SHADER] = fragment_src;
	compile(sources);
}

OpenGLShader::~OpenGLShader() {
	glDeleteProgram(renderer_id_);
}

std::string OpenGLShader::readFile(const std::string& filepath) {
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	if (in) {
		in.seekg(0, std::ios::end);
		size_t size = in.tellg();
		if (size != -1) {
			result.resize(size);
			in.seekg(0, std::ios::beg);
			in.read(result.data(), size);
			in.close();
		} else {
			HZ_CORE_ERROR("Could not read from file '{0}'", filepath);
		}
	} else {
		HZ_CORE_ERROR("Could not open file '{0}'", filepath);
	}
	return result;
}

std::unordered_map<GLenum, std::string> OpenGLShader::preProcess(const std::string& source) {
	std::unordered_map<GLenum, std::string> shader_sources;
	const char* type_token = "#type";
	size_t type_token_length = strlen(type_token);
	size_t pos = source.find(type_token, 0);
	while (pos != std::string::npos) {
		size_t eol = source.find_first_of("\r\n", pos);
		HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");
		size_t begin = pos + type_token_length + 1;
		std::string type = source.substr(begin, eol - begin);
		HZ_CORE_ASSERT(shaderTypeFromString(type), "Invalid shader type specified");

		size_t next_line_pos = source.find_first_not_of("\r\n", eol);
		pos = source.find(type_token, next_line_pos);
		shader_sources[shaderTypeFromString(type)] = source.substr(
		    next_line_pos, pos - (next_line_pos == std::string::npos ? source.size() - 1 : next_line_pos));
	}
	return shader_sources;
}

void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shader_sources) {
	GLuint program = glCreateProgram();
	HZ_CORE_ASSERT(shader_sources.size() <= 2, "We only support 2 shaders for now");
	std::array<GLenum, 2> gl_shader_ids;
	int gl_shader_id_index = 0;
	for (const auto& kv : shader_sources) {
		GLenum type = kv.first;
		const std::string& source = kv.second;
		GLuint shader = glCreateShader(type);
		const GLchar* source_c_str = source.c_str();
		glShaderSource(shader, 1, &source_c_str, nullptr);
		glCompileShader(shader);
		GLint is_compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
		if (is_compiled == GL_FALSE) {
			GLint max_length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);
			std::vector<GLchar> info_log(max_length);
			glGetShaderInfoLog(shader, max_length, &max_length, info_log.data());
			glDeleteShader(shader);
			HZ_CORE_ERROR("{0}", info_log.data());
			HZ_CORE_ASSERT(false, "Shader compilation failure!");
			break;
		}
		glAttachShader(program, shader);
		gl_shader_ids[gl_shader_id_index++] = shader;
	}

	renderer_id_ = program;

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

		for (auto id : gl_shader_ids) {
			glDeleteShader(id);
		}

		HZ_CORE_ERROR("{0}", info_log.data());
		HZ_CORE_ASSERT(false, "Shader link failure!");
		return;
	}

	for (auto id : gl_shader_ids) {
		glDetachShader(program, id);
		glDeleteShader(id);
	}
}

void OpenGLShader::bind() const {
	glUseProgram(renderer_id_);
}

void OpenGLShader::unBind() const {
	glUseProgram(0);
}

void OpenGLShader::setInt(const std::string& name, int value) {
	uploadUniformInt(name, value);
}

void OpenGLShader::setFloat3(const std::string& name, const glm::vec3& value) {
	uploadUniformFloat3(name, value);
}

void OpenGLShader::setFloat4(const std::string& name, const glm::vec4& value) {
	uploadUniformFloat4(name, value);
}

void OpenGLShader::setMat4(const std::string& name, const glm::mat4& value) {
	uploadUniformMat4(name, value);
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