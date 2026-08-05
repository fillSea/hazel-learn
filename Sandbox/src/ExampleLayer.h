#pragma once
#include <Hazel.h>

#include "hazel/renderer/Shader.h"

class ExampleLayer : public hazel::Layer {
public:
	ExampleLayer();

	void onUpdate(hazel::Timestep ts) override;

	void onEvent(hazel::Event& e) override;

	void onImGuiRender() override;

private:
	hazel::ShaderLibrary shader_library_;

	hazel::Ref<hazel::Shader> shader_;
	hazel::Ref<hazel::VertexArray> vertex_array_;

	hazel::Ref<hazel::Shader> flat_color_shader_;
	hazel::Ref<hazel::VertexArray> square_va_;
	hazel::Ref<hazel::Texture2D> texture_;
	hazel::Ref<hazel::Texture2D> cherno_logo_texture_;

	hazel::OrthographicCameraController camera_controller_;

	glm::vec3 square_color_{0.2f, 0.3f, 0.8f};
};