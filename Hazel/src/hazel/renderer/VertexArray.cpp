#include "VertexArray.h"

#include "Renderer.h"
#include "RendererAPI.h"
#include "platform/OpenGL/OpenGLVertexArray.h"

namespace hazel {

VertexArray* VertexArray::create() {
	switch (Renderer::getAPI()) {
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLVertexArray();
	}

	HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

}  // namespace hazel