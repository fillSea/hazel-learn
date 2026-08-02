#include "Renderer2D.h"

#include <glm/gtc/matrix_transform.hpp>

#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"

namespace hazel {

struct Renderer2DStorage {
	Ref<VertexArray> quad_vertex_array;
	Ref<Shader> texture_shader;
	Ref<Texture2D> white_texture;
};

static Renderer2DStorage* g_s_data;

void Renderer2D::init() {
	g_s_data = new Renderer2DStorage();
	g_s_data->quad_vertex_array = VertexArray::create();

	float square_vertices[5 * 4] = {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.0f,
	                                0.5f,  0.5f,  0.0f, 1.0f, 1.0f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f};

	Ref<VertexBuffer> square_vb;
	square_vb.reset(VertexBuffer::create(square_vertices, sizeof(square_vertices)));
	square_vb->setLayout({{ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float2, "a_TexCoord"}});
	g_s_data->quad_vertex_array->addVertexBuffer(square_vb);

	uint32_t square_indices[6] = {0, 1, 2, 2, 3, 0};
	Ref<IndexBuffer> square_ib;
	square_ib.reset(IndexBuffer::create(square_indices, sizeof(square_indices) / sizeof(uint32_t)));
	g_s_data->quad_vertex_array->setIndexBuffer(square_ib);

	g_s_data->white_texture = Texture2D::create(1, 1);
	uint32_t white_texture_data = 0xffffffff;
	g_s_data->white_texture->setData(&white_texture_data, sizeof(uint32_t));

	g_s_data->texture_shader = Shader::create("assets/shaders/Texture.glsl");
	g_s_data->texture_shader->bind();
	g_s_data->texture_shader->setInt("u_Texture", 0);
}

void Renderer2D::shutdown() {
	delete g_s_data;
}

void Renderer2D::beginScene(const OrthographicCamera& camera) {
	g_s_data->texture_shader->bind();
	g_s_data->texture_shader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());
}

void Renderer2D::endScene() {}

void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
	drawQuad({position.x, position.y, 0.0f}, size, color);
}

void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
	g_s_data->texture_shader->setFloat4("u_Color", color);
	g_s_data->white_texture->bind();

	glm::mat4 transform =
	    glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
	g_s_data->texture_shader->setMat4("u_Transform", transform);

	g_s_data->quad_vertex_array->bind();
	RenderCommand::drawIndexed(g_s_data->quad_vertex_array);
}

void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture) {
	drawQuad({position.x, position.y, 0.0f}, size, texture);
}

void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture) {
	g_s_data->texture_shader->setFloat4("u_Color", glm::vec4(1.0f));
	texture->bind();

	glm::mat4 transform =
	    glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
	g_s_data->texture_shader->setMat4("u_Transform", transform);

	g_s_data->quad_vertex_array->bind();
	RenderCommand::drawIndexed(g_s_data->quad_vertex_array);
}

}  // namespace hazel