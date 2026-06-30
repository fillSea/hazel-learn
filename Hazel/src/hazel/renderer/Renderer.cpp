#include "Renderer.h"

#include "RenderCommand.h"

namespace hazel {
void Renderer::beginScene() {}

void Renderer::endScene() {}

void Renderer::submit(const std::shared_ptr<VertexArray>& vertex_array) {
	vertex_array->bind();
	RenderCommand::drawIndexed(vertex_array);
}
}  // namespace hazel