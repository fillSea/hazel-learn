#include "Shader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "platform/OpenGL/OpenGLShader.h"

namespace hazel {
Shader::Shader(const std::string& vertex_src, const std::string& fragment_src) {}

Ref<Shader> Shader::create(const std::string& filepath) {
	switch (Renderer::getAPI()) {
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(filepath);
	}

	HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

Ref<Shader> Shader::create(const std::string& name, const std::string& vertex_src, const std::string& fragment_src) {
	switch (Renderer::getAPI()) {
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(name, vertex_src, fragment_src);
	}

	HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

// ----------------------------- ShaderLibrary -----------------------------
void ShaderLibrary::add(const std::string& name, const Ref<Shader>& shader) {
	HZ_CORE_ASSERT(!isExist(name), "Shader already exists!");
	shaders_[name] = shader;
}

void ShaderLibrary::add(const Ref<Shader>& shader) {
	const auto& name = shader->getName();
	add(name, shader);
}

Ref<Shader> ShaderLibrary::load(const std::string& filepath) {
	auto shader = Shader::create(filepath);
	add(shader);
	return shader;
}

Ref<Shader> ShaderLibrary::load(const std::string& name, const std::string& filepath) {
	auto shader = Shader::create(filepath);
	add(name, shader);
	return shader;
}

Ref<Shader> ShaderLibrary::get(const std::string& name) {
	HZ_CORE_ASSERT(isExist(name), "Shader not found!");
	return shaders_[name];
}

bool ShaderLibrary::isExist(const std::string& name) const {
	return shaders_.find(name) != shaders_.end();
}
}  // namespace hazel