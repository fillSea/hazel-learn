#include "Sandbox2D.h"

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D"), camera_controller_(1280.0f / 720.0f), square_color_({0.2f, 0.3f, 0.8f, 1.0f}) {}

void Sandbox2D::onAttach() {
	HZ_PROFILE_FUNCTION();

	checker_board_texture_ = hazel::Texture2D::create("assets/textures/Checkerboard.png");

	hazel::FramebufferSpecification fb_spec;
	fb_spec.width = 1280;
	fb_spec.height = 720;
	frame_buffer_ = hazel::Framebuffer::create(fb_spec);
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
		frame_buffer_->bind();
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
		frame_buffer_->unbind();
	}
}

void Sandbox2D::onImGuiRender() {
	HZ_PROFILE_FUNCTION();

	// Note: Switch this to true to enable dockspace
	static bool s_docking_enabled = true;
	if (s_docking_enabled) {
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
					hazel::Application::getInstance().close();
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Begin("Settings");

		auto stats = hazel::Renderer2D::getStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.draw_calls);
		ImGui::Text("Quads: %d", stats.quad_count);
		ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
		ImGui::Text("Indices: %d", stats.getTotalIndexCount());

		ImGui::ColorEdit4("Square Color", glm::value_ptr(square_color_));

		uint32_t texture_id = frame_buffer_->getColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2{1280, 720});
		ImGui::End();

		ImGui::End();
	} else {
		ImGui::Begin("Settings");

		auto stats = hazel::Renderer2D::getStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.draw_calls);
		ImGui::Text("Quads: %d", stats.quad_count);
		ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
		ImGui::Text("Indices: %d", stats.getTotalIndexCount());

		ImGui::ColorEdit4("Square Color", glm::value_ptr(square_color_));

		uint32_t texture_id = checker_board_texture_->getRendererID();
		ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2{1280, 720});
		ImGui::End();
	}
}

void Sandbox2D::onEvent(hazel::Event& e) {
	camera_controller_.onEvent(e);
}
