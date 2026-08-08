#include "Sandbox2D.h"

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), camera_controller_(1280.0f / 720.0f) {}

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
	{
		HZ_PROFILE_SCOPE("Renderer Prep");
		hazel::RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 1});
		hazel::RenderCommand::clear();
	}

	{
		HZ_PROFILE_SCOPE("Renderer Draw");
		hazel::Renderer2D::beginScene(camera_controller_.getCamera());
		hazel::Renderer2D::drawRotatedQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, glm::radians(-45.0f), {0.8f, 0.2f, 0.3f, 1.0f});
		hazel::Renderer2D::drawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
		hazel::Renderer2D::drawQuad({0.0f, 0.0f, -0.1f}, {10.0f, 10.0f}, checker_board_texture_, 10.0f);
		hazel::Renderer2D::endScene();
	}
}

void Sandbox2D::onImGuiRender() {
	HZ_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(square_color_));

	ImGui::End();
}

void Sandbox2D::onEvent(hazel::Event& e) {
	camera_controller_.onEvent(e);
}
