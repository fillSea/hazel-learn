#include "Sandbox2D.h"

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), camera_controller_(1280.0f / 720.0f) {}

void Sandbox2D::onAttach() {
	square_va_ = hazel::VertexArray::create();

	float square_vertices[5 * 4] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, -0.5f, 0.5f, 0.0f};

	hazel::Ref<hazel::VertexBuffer> square_vb;
	square_vb.reset(hazel::VertexBuffer::create(square_vertices, sizeof(square_vertices)));
	square_vb->setLayout({{hazel::ShaderDataType::Float3, "a_Position"}});
	square_va_->addVertexBuffer(square_vb);

	uint32_t square_indices[6] = {0, 1, 2, 2, 3, 0};
	hazel::Ref<hazel::IndexBuffer> square_ib;
	square_ib.reset(hazel::IndexBuffer::create(square_indices, sizeof(square_indices) / sizeof(uint32_t)));
	square_va_->setIndexBuffer(square_ib);

	flat_color_shader_ = hazel::Shader::create("assets/shaders/FlatColor.glsl");
}

void Sandbox2D::onDetach() {}

void Sandbox2D::onUpdate(hazel::Timestep ts) {
	// Update
	camera_controller_.onUpdate(ts);

	// Render
	hazel::RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 1});
	hazel::RenderCommand::clear();

	hazel::Renderer::beginScene(camera_controller_.getCamera());

	std::dynamic_pointer_cast<hazel::OpenGLShader>(flat_color_shader_)->bind();
	std::dynamic_pointer_cast<hazel::OpenGLShader>(flat_color_shader_)->uploadUniformFloat4("u_Color", square_color_);
	hazel::Renderer::submit(flat_color_shader_, square_va_);

	hazel::Renderer::submit(flat_color_shader_, square_va_);

	hazel::Renderer::endScene();
}

void Sandbox2D::onImGuiRender() {
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(square_color_));
	ImGui::End();
}

void Sandbox2D::onEvent(hazel::Event& e) {
	camera_controller_.onEvent(e);
}
