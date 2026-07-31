#include "Renderer.h"

#include "RenderCommand.h"
#include "platform/OpenGL/OpenGLShader.h"

namespace hazel {
Scope<Renderer::SceneData> Renderer::s_scene_data_ = createScope<Renderer::SceneData>();

void Renderer::init() {
	RenderCommand::init();
}

void Renderer::beginScene(OrthographicCamera& camera) {
	s_scene_data_->view_projection_matrix = camera.getViewProjectionMatrix();
}

void Renderer::endScene() {}

void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertex_array,
                      const glm::mat4& transform) {
	shader->bind();

	std::dynamic_pointer_cast<OpenGLShader>(shader)->uploadUniformMat4("u_ViewProjection",
	                                                                   s_scene_data_->view_projection_matrix);
	std::dynamic_pointer_cast<OpenGLShader>(shader)->uploadUniformMat4("u_Transform", transform);

	vertex_array->bind();
	RenderCommand::drawIndexed(vertex_array);
}

void Renderer::onWindowResize(uint32_t width, uint32_t height) {
	RenderCommand::setViewport(0, 0, width, height);
}
}  // namespace hazel