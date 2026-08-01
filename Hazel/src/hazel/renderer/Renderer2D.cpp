#include "Renderer2D.h"

#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"
#include "platform/OpenGL/OpenGLShader.h"

namespace hazel {

struct Renderer2DStorage {
	Ref<VertexArray> quad_vertex_array;
	Ref<Shader> flat_color_shader;
};

static Renderer2DStorage* g_s_data;

void Renderer2D::init() {
	g_s_data = new Renderer2DStorage();
	g_s_data->quad_vertex_array = VertexArray::create();

	float square_vertices[5 * 4] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, -0.5f, 0.5f, 0.0f};

	Ref<VertexBuffer> square_vb;
	square_vb.reset(VertexBuffer::create(square_vertices, sizeof(square_vertices)));
	square_vb->setLayout({{ShaderDataType::Float3, "a_Position"}});
	g_s_data->quad_vertex_array->addVertexBuffer(square_vb);

	uint32_t square_indices[6] = {0, 1, 2, 2, 3, 0};
	Ref<IndexBuffer> square_ib;
	square_ib.reset(IndexBuffer::create(square_indices, sizeof(square_indices) / sizeof(uint32_t)));
	g_s_data->quad_vertex_array->setIndexBuffer(square_ib);

	g_s_data->flat_color_shader = Shader::create("assets/shaders/FlatColor.glsl");
}

void Renderer2D::shutdown() {
	delete g_s_data;
}

void Renderer2D::beginScene(const OrthographicCamera& camera) {
	std::dynamic_pointer_cast<OpenGLShader>(g_s_data->flat_color_shader)->bind();
	std::dynamic_pointer_cast<OpenGLShader>(g_s_data->flat_color_shader)
	    ->uploadUniformMat4("u_ViewProjection", camera.getViewProjectionMatrix());
	std::dynamic_pointer_cast<OpenGLShader>(g_s_data->flat_color_shader)
	    ->uploadUniformMat4("u_Transform", glm::mat4(1.0f));
}

void Renderer2D::endScene() {}

void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
	drawQuad({position.x, position.y, 0.0f}, size, color);
}

void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
	std::dynamic_pointer_cast<OpenGLShader>(g_s_data->flat_color_shader)->bind();
	std::dynamic_pointer_cast<OpenGLShader>(g_s_data->flat_color_shader)->uploadUniformFloat4("u_Color", color);

	g_s_data->quad_vertex_array->bind();
	RenderCommand::drawIndexed(g_s_data->quad_vertex_array);
}

}  // namespace hazel