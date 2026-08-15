#pragma once

#include <Hazel.h>

namespace hazel {

class EditorLayer : public Layer {
public:
	EditorLayer();
	~EditorLayer() override = default;

	EditorLayer(const EditorLayer&) = delete;
	EditorLayer& operator=(const EditorLayer&) = delete;
	EditorLayer(EditorLayer&&) = delete;
	EditorLayer& operator=(EditorLayer&&) = delete;

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
	hazel::Ref<hazel::Framebuffer> frame_buffer_;

	Ref<Scene> active_scene_;
	Entity square_entity_;
	Entity camera_entity_;
	Entity second_camera_;

	bool primary_camera_ = true;

	hazel::Ref<hazel::Texture2D> checker_board_texture_;

	glm::vec2 viewport_size_{0.0f, 0.0f};
	bool viewport_focused_ = false;
	bool viewport_hovered_ = false;

	glm::vec4 square_color_{0.2f, 0.3f, 0.8f, 1.0f};
};

}  // namespace hazel