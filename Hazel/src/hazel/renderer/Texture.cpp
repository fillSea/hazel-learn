#include "Texture.h"

#include "Renderer.h"
#include "platform/OpenGL/OpenGLTexture.h"

namespace hazel {

Ref<Texture2D> Texture2D::create(uint32_t width, uint32_t height) {
	switch (Renderer::getAPI()) {
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return createRef<OpenGLTexture2D>(width, height);
	}

	HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

Ref<Texture2D> Texture2D::create(const std::string& path) {
	switch (Renderer::getAPI()) {
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return createRef<OpenGLTexture2D>(path);
	}

	HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

}  // namespace hazel