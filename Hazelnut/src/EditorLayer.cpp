#include "EditorLayer.h"

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace hazel {

EditorLayer::EditorLayer()
    : Layer("EditorLayer"), camera_controller_(1280.0f / 720.0f), square_color_({0.2f, 0.3f, 0.8f, 1.0f}) {}

void EditorLayer::onAttach() {
	HZ_PROFILE_FUNCTION();

	checker_board_texture_ = Texture2D::create("assets/textures/Checkerboard.png");

	FramebufferSpecification fb_spec;
	fb_spec.width = 1280;
	fb_spec.height = 720;
	frame_buffer_ = Framebuffer::create(fb_spec);

	active_scene_ = createRef<Scene>();

	auto square = active_scene_->createEntity();
	active_scene_->reg().emplace<TransformComponent>(square);
	active_scene_->reg().emplace<SpriteRendererComponent>(square, glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});

	square_entity_ = square;
}

void EditorLayer::onDetach() {
	HZ_PROFILE_FUNCTION();
}

void EditorLayer::onUpdate(Timestep ts) {
	HZ_PROFILE_FUNCTION();

	// Resize
	if (FramebufferSpecification spec = frame_buffer_->getSpecification();
	    viewport_size_.x > 0.0f && viewport_size_.y > 0.0f &&  // zero sized framebuffer is invalid
	    (spec.width != viewport_size_.x || spec.height != viewport_size_.y)) {
		frame_buffer_->resize(static_cast<uint32_t>(viewport_size_.x), static_cast<uint32_t>(viewport_size_.y));
		camera_controller_.onResize(viewport_size_.x, viewport_size_.y);
	}

	// Update
	if (viewport_focused_) {
		camera_controller_.onUpdate(ts);
	}

	// Render
	Renderer2D::resetStats();
	frame_buffer_->bind();
	RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 1});
	RenderCommand::clear();

	Renderer2D::beginScene(camera_controller_.getCamera());

	// Update scene
	active_scene_->onUpdate(ts);

	Renderer2D::endScene();

	frame_buffer_->unbind();
}

void EditorLayer::onImGuiRender() {
	HZ_PROFILE_FUNCTION();

	// Note: Switch this to true to enable dockspace
	static bool s_dockspace_open = true;
	static bool s_opt_fullscreen_persistant = true;
	bool opt_fullscreen = s_opt_fullscreen_persistant;
	static ImGuiDockNodeFlags s_dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen) {
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
		                ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the
	// pass-thru hole, so we ask Begin() to not render a background.
	if (s_dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
		window_flags |= ImGuiWindowFlags_NoBackground;
	}

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &s_dockspace_open, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen) {
		ImGui::PopStyleVar(2);
	}

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), s_dockspace_flags);
	}

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			// Disabling fullscreen would allow the window to be moved to the front of other windows,
			// which we can't undo at the moment without finer window depth/z control.
			// ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

			if (ImGui::MenuItem("Exit")) {
				Application::getInstance().close();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::Begin("Settings");

	auto stats = Renderer2D::getStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.draw_calls);
	ImGui::Text("Quads: %d", stats.quad_count);
	ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
	ImGui::Text("Indices: %d", stats.getTotalIndexCount());

	auto& square_color = active_scene_->reg().get<SpriteRendererComponent>(square_entity_).color;
	ImGui::ColorEdit4("Square Color", glm::value_ptr(square_color));

	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
	ImGui::Begin("Viewport");

	viewport_focused_ = ImGui::IsWindowFocused();
	viewport_hovered_ = ImGui::IsWindowHovered();
	Application::getInstance().getImGuiLayer()->blockEvents(!viewport_focused_ || !viewport_hovered_);

	ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
	viewport_size_ = {viewport_panel_size.x, viewport_panel_size.y};
	uint32_t texture_id = frame_buffer_->getColorAttachmentRendererID();
	ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2{viewport_size_.x, viewport_size_.y}, ImVec2{0, 1},
	             ImVec2{1, 0});
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::End();
}

void EditorLayer::onEvent(Event& e) {
	camera_controller_.onEvent(e);
}

}  // namespace hazel