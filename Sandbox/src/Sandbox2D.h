#pragma once

#include <Hazel.h>

class Sandbox2D : public hazel::Layer {
public:
	Sandbox2D();
	~Sandbox2D() override = default;

	Sandbox2D(const Sandbox2D&) = delete;
	Sandbox2D& operator=(const Sandbox2D&) = delete;
	Sandbox2D(Sandbox2D&&) = delete;
	Sandbox2D& operator=(Sandbox2D&&) = delete;

	void onAttach() override;
	void onDetach() override;

	void onUpdate(hazel::Timestep ts) override;
	void onImGuiRender() override;
	void onEvent(hazel::Event& e) override;

private:
	hazel::OrthographicCameraController camera_controller_;

	// Temp
	hazel::Ref<hazel::VertexArray> square_va_;
	hazel::Ref<hazel::Shader> flat_color_shader_;

	hazel::Ref<hazel::Texture2D> checker_board_texture_;

	glm::vec4 square_color_ = {0.2f, 0.3f, 0.8f, 1.0f};
};