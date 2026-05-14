#include "ImGuiLayer.h"

#include <GLFW/glfw3.h>

#include "hazel/Application.h"
#include "imgui.h"
#include "platform/OpenGL/ImGuiOpenGLRenderer.h"

namespace hazel {
ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::onAttach() {
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

	ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::onDetach() {}

void ImGuiLayer::onUpdate() {
	ImGuiIO& io = ImGui::GetIO();
	Application& app = Application::getInstance();
	io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

	float time = static_cast<float>(glfwGetTime());
	io.DeltaTime = time_ > 0.0F ? (time - time_) : (1.0F / 60.0F);
	time_ = time;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	static bool show = true;
	ImGui::ShowDemoWindow(&show);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::onEvent(Event& event) {}
}  // namespace hazel