#include "Renderer.h"

#include "RenderCommand.h"

namespace hazel {
Renderer::SceneData* Renderer::s_scene_data_ = new Renderer::SceneData();

void Renderer::beginScene(OrthographicCamera& camera) {
	s_scene_data_->view_projection_matrix = camera.getViewProjectionMatrix();
}

void Renderer::endScene() {}

void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertex_array,
                      const glm::mat4& transform) {
	shader->bind();
	// 将视图投影矩阵上传至着色器的 u_ViewProjection uniform
	shader->uploadUniformMat4("u_ViewProjection", s_scene_data_->view_projection_matrix);
	shader->uploadUniformMat4("u_Transform", transform);

	vertex_array->bind();
	RenderCommand::drawIndexed(vertex_array);
}
}  // namespace hazel