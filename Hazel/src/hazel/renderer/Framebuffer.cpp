#include "Framebuffer.h"

#include "hazel/renderer/Renderer.h"
#include "platform/OpenGL/OpenGLFramebuffer.h"

namespace hazel {

Ref<Framebuffer> Framebuffer::create(const FramebufferSpecification& spec) {
	switch (Renderer::getAPI()) {
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return createRef<OpenGLFramebuffer>(spec);
	}

	HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

}  // namespace hazel
