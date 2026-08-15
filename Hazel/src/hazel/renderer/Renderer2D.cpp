#include "Renderer2D.h"

#include <glm/gtc/matrix_transform.hpp>

#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"

namespace hazel {

/// @brief 四边形顶点数据结构
struct QuadVertex {
	glm::vec3 position;   /// 位置
	glm::vec4 color;      /// 颜色
	glm::vec2 tex_coord;  /// 纹理坐标
	float tex_index;      /// 纹理索引
	float tiling_factor;  /// 纹理平铺因子
};

/// @brief 2D 渲染器内部存储结构
struct Renderer2DData {
	/// 最大绘制图元数量
	static const uint32_t k_max_quads = 20000;
	/// 最大顶点数量
	static const uint32_t k_max_vertices = k_max_quads * 4;
	/// 最大索引数量
	static const uint32_t k_max_indices = k_max_quads * 6;
	/// 最大纹理槽数量
	static const uint32_t k_max_texture_slots = 32;  // TODO: RenderCaps

	/// @brief 四边形顶点数组对象
	Ref<VertexArray> quad_vertex_array;
	/// @brief 四边形顶点缓冲对象
	Ref<VertexBuffer> quad_vertex_buffer;
	/// @brief 四边形着色器
	Ref<Shader> texture_shader;
	/// @brief 白色纹理，用于绘制纯色四边形
	Ref<Texture2D> white_texture;

	/// @brief 当前绘制的索引数量
	uint32_t quad_index_count = 0;
	/// @brief 顶点缓冲数据指针，用于批量绘制, 基准指针
	QuadVertex* quad_vertex_buffer_base = nullptr;
	/// @brief 顶点缓冲数据指针，用于批量绘制
	QuadVertex* quad_vertex_buffer_ptr = nullptr;

	/// @brief 纹理槽数组，用于存储当前绑定的纹理
	std::array<Ref<Texture2D>, k_max_texture_slots> texture_slots;
	/// @brief 当前纹理槽索引，0 号槽为白色纹理
	uint32_t texture_slot_index = 1;  // 0 = white texture

	/// @brief 四边形顶点位置数组，用于计算顶点坐标
	glm::vec4 quad_vertex_positions[4];

	/// @brief 渲染统计信息
	Renderer2D::Statistics stats;
};

static Renderer2DData g_s_data;

void Renderer2D::init() {
	HZ_PROFILE_FUNCTION();

	g_s_data.quad_vertex_array = VertexArray::create();

	g_s_data.quad_vertex_buffer = VertexBuffer::create(g_s_data.k_max_vertices * sizeof(QuadVertex));
	g_s_data.quad_vertex_buffer->setLayout({{ShaderDataType::Float3, "a_Position"},
	                                        {ShaderDataType::Float4, "a_Color"},
	                                        {ShaderDataType::Float2, "a_TexCoord"},
	                                        {ShaderDataType::Float, "a_TexIndex"},
	                                        {ShaderDataType::Float, "a_TilingFactor"}});
	g_s_data.quad_vertex_array->addVertexBuffer(g_s_data.quad_vertex_buffer);

	g_s_data.quad_vertex_buffer_base = new QuadVertex[g_s_data.k_max_vertices];

	uint32_t* quad_indices = new uint32_t[g_s_data.k_max_indices];

	uint32_t offset = 0;
	for (uint32_t i = 0; i < g_s_data.k_max_indices; i += 6) {
		quad_indices[i + 0] = offset + 0;
		quad_indices[i + 1] = offset + 1;
		quad_indices[i + 2] = offset + 2;

		quad_indices[i + 3] = offset + 2;
		quad_indices[i + 4] = offset + 3;
		quad_indices[i + 5] = offset + 0;

		offset += 4;
	}

	Ref<IndexBuffer> quad_ib{IndexBuffer::create(quad_indices, g_s_data.k_max_indices)};
	g_s_data.quad_vertex_array->setIndexBuffer(quad_ib);
	delete[] quad_indices;

	g_s_data.white_texture = Texture2D::create(1, 1);
	uint32_t white_texture_data = 0xffffffff;
	g_s_data.white_texture->setData(&white_texture_data, sizeof(uint32_t));

	int32_t samplers[g_s_data.k_max_texture_slots];
	for (uint32_t i = 0; i < g_s_data.k_max_texture_slots; i++) {
		samplers[i] = i;
	}

	g_s_data.texture_shader = Shader::create("assets/shaders/Texture.glsl");
	g_s_data.texture_shader->bind();
	g_s_data.texture_shader->setIntArray("u_Textures", samplers, g_s_data.k_max_texture_slots);

	// Set all texture slots to 0
	g_s_data.texture_slots[0] = g_s_data.white_texture;

	g_s_data.quad_vertex_positions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
	g_s_data.quad_vertex_positions[1] = {0.5f, -0.5f, 0.0f, 1.0f};
	g_s_data.quad_vertex_positions[2] = {0.5f, 0.5f, 0.0f, 1.0f};
	g_s_data.quad_vertex_positions[3] = {-0.5f, 0.5f, 0.0f, 1.0f};
}

void Renderer2D::shutdown() {
	HZ_PROFILE_FUNCTION();

	delete[] g_s_data.quad_vertex_buffer_base;
}

void Renderer2D::beginScene(const OrthographicCamera& camera) {
	HZ_PROFILE_FUNCTION();

	g_s_data.texture_shader->bind();
	g_s_data.texture_shader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());

	g_s_data.quad_index_count = 0;
	g_s_data.quad_vertex_buffer_ptr = g_s_data.quad_vertex_buffer_base;
	g_s_data.texture_slot_index = 1;
}

void Renderer2D::endScene() {
	HZ_PROFILE_FUNCTION();

	uint32_t data_size = reinterpret_cast<uint8_t*>(g_s_data.quad_vertex_buffer_ptr) -
	                     reinterpret_cast<uint8_t*>(g_s_data.quad_vertex_buffer_base);
	g_s_data.quad_vertex_buffer->setData(g_s_data.quad_vertex_buffer_base, data_size);

	flush();
}

void Renderer2D::flush() {
	if (g_s_data.quad_index_count == 0) {
		return;  // Nothing to draw
	}
	// Bind textures
	for (uint32_t i = 0; i < g_s_data.texture_slot_index; i++) {
		g_s_data.texture_slots[i]->bind(i);
	}

	RenderCommand::drawIndexed(g_s_data.quad_vertex_array, g_s_data.quad_index_count);

	g_s_data.stats.draw_calls++;
}

void Renderer2D::flushAndReset() {
	endScene();

	g_s_data.quad_index_count = 0;
	g_s_data.quad_vertex_buffer_ptr = g_s_data.quad_vertex_buffer_base;

	g_s_data.texture_slot_index = 1;
}

void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
	drawQuad({position.x, position.y, 0.0f}, size, color);
}

void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
	HZ_PROFILE_FUNCTION();

	glm::mat4 transform =
	    glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

	drawQuad(transform, color);
}

void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture,
                          float tiling_factor, const glm::vec4& tint_color) {
	drawQuad({position.x, position.y, 0.0f}, size, texture, tiling_factor, tint_color);
}

void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture,
                          float tiling_factor, const glm::vec4& tint_color) {
	HZ_PROFILE_FUNCTION();

	glm::mat4 transform =
	    glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

	drawQuad(transform, texture, tiling_factor);
}

void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color) {
	HZ_PROFILE_FUNCTION();

	constexpr size_t k_quad_vertex_count = 4;
	const float k_texture_index = 0.0f;  // White Texture
	constexpr glm::vec2 k_texture_coords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
	const float k_tiling_factor = 1.0f;

	if (g_s_data.quad_index_count >= Renderer2DData::k_max_indices) {
		flushAndReset();
	}

	for (size_t i = 0; i < k_quad_vertex_count; i++) {
		g_s_data.quad_vertex_buffer_ptr->position = transform * g_s_data.quad_vertex_positions[i];
		g_s_data.quad_vertex_buffer_ptr->color = color;
		g_s_data.quad_vertex_buffer_ptr->tex_coord = k_texture_coords[i];
		g_s_data.quad_vertex_buffer_ptr->tex_index = k_texture_index;
		g_s_data.quad_vertex_buffer_ptr->tiling_factor = k_tiling_factor;
		g_s_data.quad_vertex_buffer_ptr++;
	}

	g_s_data.quad_index_count += 6;

	g_s_data.stats.quad_count++;
}

void Renderer2D::drawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tiling_factor,
                          const glm::vec4& tint_color) {
	HZ_PROFILE_FUNCTION();

	constexpr size_t k_quad_vertex_count = 4;
	constexpr glm::vec2 k_texture_coords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

	if (g_s_data.quad_index_count >= Renderer2DData::k_max_indices) {
		flushAndReset();
	}

	float texture_index = 0.0f;
	for (uint32_t i = 1; i < g_s_data.texture_slot_index; i++) {
		if (*g_s_data.texture_slots[i] == *texture.get()) {
			texture_index = static_cast<float>(i);
			break;
		}
	}

	if (texture_index == 0.0f) {
		if (g_s_data.texture_slot_index >= Renderer2DData::k_max_texture_slots) {
			flushAndReset();
		}

		texture_index = static_cast<float>(g_s_data.texture_slot_index);
		g_s_data.texture_slots[g_s_data.texture_slot_index] = texture;
		g_s_data.texture_slot_index++;
	}

	for (size_t i = 0; i < k_quad_vertex_count; i++) {
		g_s_data.quad_vertex_buffer_ptr->position = transform * g_s_data.quad_vertex_positions[i];
		g_s_data.quad_vertex_buffer_ptr->color = tint_color;
		g_s_data.quad_vertex_buffer_ptr->tex_coord = k_texture_coords[i];
		g_s_data.quad_vertex_buffer_ptr->tex_index = texture_index;
		g_s_data.quad_vertex_buffer_ptr->tiling_factor = tiling_factor;
		g_s_data.quad_vertex_buffer_ptr++;
	}

	g_s_data.quad_index_count += 6;

	g_s_data.stats.quad_count++;
}

void Renderer2D::drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation,
                                 const glm::vec4& color) {
	drawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, color);
}

void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation,
                                 const glm::vec4& color) {
	HZ_PROFILE_FUNCTION();

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
	                      glm::rotate(glm::mat4(1.0f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) *
	                      glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

	drawQuad(transform, color);
}

void Renderer2D::drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation,
                                 const Ref<Texture2D>& texture, float tiling_factor, const glm::vec4& tint_color) {
	drawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, texture, tiling_factor, tint_color);
}

void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation,
                                 const Ref<Texture2D>& texture, float tiling_factor, const glm::vec4& tint_color) {
	HZ_PROFILE_FUNCTION();

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
	                      glm::rotate(glm::mat4(1.0f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) *
	                      glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

	drawQuad(transform, texture, tiling_factor, tint_color);
}

void Renderer2D::resetStats() {
	memset(&g_s_data.stats, 0, sizeof(Statistics));
}

Renderer2D::Statistics Renderer2D::getStats() {
	return g_s_data.stats;
}

}  // namespace hazel