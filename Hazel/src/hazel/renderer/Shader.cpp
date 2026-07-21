#include "Shader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "platform/OpenGL/OpenGLShader.h"

namespace hazel {
Shader::Shader(const std::string& vertex_src, const std::string& fragment_src) {}

Shader* Shader::create(const std::string& filepath) {
	switch (Renderer::getAPI()) {
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLShader(filepath);
	}

	HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

Shader* Shader::create(const std::string& vertex_src, const std::string& fragment_src) {
	switch (Renderer::getAPI()) {
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLShader(vertex_src, fragment_src);
	}

	HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}
}  // namespace hazel