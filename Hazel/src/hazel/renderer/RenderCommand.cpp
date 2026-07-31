#include "RenderCommand.h"

#include "platform/OpenGL/OpenGLRendererAPI.h"

namespace hazel {

Scope<RendererAPI> RenderCommand::s_renderer_api_ = createScope<OpenGLRendererAPI>();

}