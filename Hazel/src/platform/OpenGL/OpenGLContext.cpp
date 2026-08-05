#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace hazel {

OpenGLContext::OpenGLContext(GLFWwindow* window_handle) : window_handle_(window_handle) {
	HZ_CORE_ASSERT(window_handle, "Window handle is null!")
}

void OpenGLContext::init() {
	HZ_PROFILE_FUNCTION();

	glfwMakeContextCurrent(window_handle_);
	int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	HZ_CORE_ASSERT(status, "Failed to initialize Glad!");

	HZ_CORE_INFO("OpenGL Info:");
	HZ_CORE_INFO("  Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	HZ_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	HZ_CORE_INFO("  Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
}

void OpenGLContext::swapBuffers() {
	HZ_PROFILE_FUNCTION();

	glfwSwapBuffers(window_handle_);
}

}  // namespace hazel