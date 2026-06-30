#include "RenderCommand.h"

#include "platform/OpenGL/OpenGLRendererAPI.h"

namespace hazel {

RendererAPI* RenderCommand::s_renderer_api_ = new OpenGLRendererAPI();

}