#pragma once

#include "hazel/renderer/GraphicsContext.h"

class GLFWwindow;

namespace hazel {

class OpenGLContext : public GraphicsContext {
public:
	explicit OpenGLContext(GLFWwindow* window_handle);

	void init() override;
	void swapBuffers() override;

private:
	GLFWwindow* window_handle_;
};

}  // namespace hazel