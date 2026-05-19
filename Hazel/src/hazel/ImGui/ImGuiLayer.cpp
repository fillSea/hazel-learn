#include "ImGuiLayer.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "hazel/Application.h"
#include "hazel/utils/KeyUtil.h"
#include "imgui.h"
#include "platform/OpenGL/ImGuiOpenGLRenderer.h"

namespace hazel {
ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

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

	static bool s_show = true;
	ImGui::ShowDemoWindow(&s_show);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::onEvent(Event& event) {
	EventDispatcher dispatcher(event);
	dispatcher.dispatch<MouseButtonPressedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::onMouseButtonPressedEvent));
	dispatcher.dispatch<MouseButtonReleasedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::onMouseButtonReleasedEvent));
	dispatcher.dispatch<MouseMovedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::onMouseMovedEvent));
	dispatcher.dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::onMouseScrolledEvent));
	dispatcher.dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::onKeyPressedEvent));
	dispatcher.dispatch<KeyTypedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::onKeyTypedEvent));
	dispatcher.dispatch<KeyReleasedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::onKeyReleasedEvent));
	dispatcher.dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::onWindowResizeEvent));
}

bool ImGuiLayer::onMouseButtonPressedEvent(MouseButtonPressedEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[e.getMouseButton()] = true;

	return false;
}

bool ImGuiLayer::onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[e.getMouseButton()] = false;

	return false;
}

bool ImGuiLayer::onMouseMovedEvent(MouseMovedEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(e.getX(), e.getY());

	return false;
}

bool ImGuiLayer::onMouseScrolledEvent(MouseScrolledEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += e.getXOffset();
	io.MouseWheel += e.getYOffset();

	return false;
}

bool ImGuiLayer::onKeyPressedEvent(KeyPressedEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	ImGuiKey imgui_key = KeyUtil::gLFWKeyToImGuiKey(e.getKeyCode());

	if (imgui_key != ImGuiKey_None) {
		io.AddKeyEvent(imgui_key, true);
	}

	return false;
}

bool ImGuiLayer::onKeyReleasedEvent(KeyReleasedEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	ImGuiKey imgui_key = KeyUtil::gLFWKeyToImGuiKey(e.getKeyCode());

	if (imgui_key != ImGuiKey_None) {
		io.AddKeyEvent(imgui_key, false);
	}

	return false;
}

bool ImGuiLayer::onKeyTypedEvent(KeyTypedEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	int keycode = e.getKeyCode();
	if (keycode > 0 && keycode < 0x10000) {
		io.AddInputCharacter(static_cast<uint16_t>(keycode));
	}

	return false;
}

bool ImGuiLayer::onWindowResizeEvent(WindowResizeEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(e.getWidth(), e.getHeight());
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	glViewport(0, 0, e.getWidth(), e.getHeight());

	return false;
}
}  // namespace hazel