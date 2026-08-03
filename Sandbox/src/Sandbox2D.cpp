#include "Sandbox2D.h"

#include <imgui.h>

#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

template <typename Fn>
class Timer {
public:
	Timer(const char* name, Fn&& func) : name_(name), func_(func), stopped_{false} {
		start_time_point_ = std::chrono::high_resolution_clock::now();
	}

	Timer(const Timer&) = delete;
	Timer(Timer&&) = delete;
	Timer& operator=(const Timer&) = delete;
	Timer& operator=(Timer&&) = delete;

	~Timer() {
		if (!stopped_) {
			stop();
		}
	}

	void stop() {
		auto end_timepoint = std::chrono::high_resolution_clock::now();

		int64_t start =
		    std::chrono::time_point_cast<std::chrono::microseconds>(start_time_point_).time_since_epoch().count();
		int64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(end_timepoint).time_since_epoch().count();

		stopped_ = true;

		float duration = (end - start) * 0.001f;
		func_({name_, duration});
	}

private:
	const char* name_;
	Fn func_;
	std::chrono::time_point<std::chrono::steady_clock> start_time_point_;
	bool stopped_;
};

#define PROFILE_SCOPE(name) \
	Timer timer##__LINE__(name, [&](ProfileResult profile_result) { profile_results_.push_back(profile_result); })

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), camera_controller_(1280.0f / 720.0f) {}

void Sandbox2D::onAttach() {
	checker_board_texture_ = hazel::Texture2D::create("assets/textures/Checkerboard.png");
}

void Sandbox2D::onDetach() {}

void Sandbox2D::onUpdate(hazel::Timestep ts) {
	PROFILE_SCOPE("Sandbox2D::OnUpdate");

	// Update
	{
		PROFILE_SCOPE("CameraController::OnUpdate");
		camera_controller_.onUpdate(ts);
	}

	// Render
	{
		PROFILE_SCOPE("Renderer Prep");
		hazel::RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 1});
		hazel::RenderCommand::clear();
	}

	{
		PROFILE_SCOPE("Renderer Draw");
		hazel::Renderer2D::beginScene(camera_controller_.getCamera());
		hazel::Renderer2D::drawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
		hazel::Renderer2D::drawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
		hazel::Renderer2D::drawQuad({0.0f, 0.0f, -0.1f}, {10.0f, 10.0f}, checker_board_texture_);
		hazel::Renderer2D::endScene();
	}
}

void Sandbox2D::onImGuiRender() {
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(square_color_));

	for (auto& result : profile_results_) {
		char label[50];
		strcpy(label, "%.3fms ");
		strcat(label, result.name);
		ImGui::Text(label, result.time);
	}
	profile_results_.clear();

	ImGui::End();
}

void Sandbox2D::onEvent(hazel::Event& e) {
	camera_controller_.onEvent(e);
}
