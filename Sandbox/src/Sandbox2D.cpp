#include "Sandbox2D.h"

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D"), camera_controller_(1280.0f / 720.0f), square_color_({0.2f, 0.3f, 0.8f, 1.0f}) {}

void Sandbox2D::onAttach() {
	HZ_PROFILE_FUNCTION();

	checker_board_texture_ = hazel::Texture2D::create("assets/textures/Checkerboard.png");
}

void Sandbox2D::onDetach() {
	HZ_PROFILE_FUNCTION();
}

void Sandbox2D::onUpdate(hazel::Timestep ts) {
	HZ_PROFILE_FUNCTION();

	// Update
	camera_controller_.onUpdate(ts);

	// Render
	hazel::Renderer2D::resetStats();
	{
		HZ_PROFILE_SCOPE("Renderer Prep");
		hazel::RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 1});
		hazel::RenderCommand::clear();
	}

	{
		static float s_rotation = 0.0f;
		s_rotation += ts * 50.0f;

		HZ_PROFILE_SCOPE("Renderer Draw");
		hazel::Renderer2D::beginScene(camera_controller_.getCamera());
		hazel::Renderer2D::drawRotatedQuad({1.0f, 0.0f}, {0.8f, 0.8f}, -45.0f, {0.8f, 0.2f, 0.3f, 1.0f});
		hazel::Renderer2D::drawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
		hazel::Renderer2D::drawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, square_color_);
		hazel::Renderer2D::drawQuad({0.0f, 0.0f, -0.1f}, {20.0f, 20.0f}, checker_board_texture_, 10.0f);
		hazel::Renderer2D::drawRotatedQuad({-2.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, s_rotation, checker_board_texture_,
		                                   20.0f);
		hazel::Renderer2D::endScene();

		hazel::Renderer2D::beginScene(camera_controller_.getCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f) {
			for (float x = -5.0f; x < 5.0f; x += 0.5f) {
				glm::vec4 color = {(x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f};
				hazel::Renderer2D::drawQuad({x, y}, {0.45f, 0.45f}, color);
			}
		}
		hazel::Renderer2D::endScene();
	}
}

void Sandbox2D::onImGuiRender() {
	HZ_PROFILE_FUNCTION();

	ImGui::Begin("Settings");

	auto stats = hazel::Renderer2D::getStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.draw_calls);
	ImGui::Text("Quads: %d", stats.quad_count);
	ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
	ImGui::Text("Indices: %d", stats.getTotalIndexCount());

	ImGui::ColorEdit4("Square Color", glm::value_ptr(square_color_));

	ImGui::End();
}

void Sandbox2D::onEvent(hazel::Event& e) {
	camera_controller_.onEvent(e);
}
