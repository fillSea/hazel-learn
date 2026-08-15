#pragma once

// for use by Hazel applications
#include "hazel/ImGui/ImGuiLayer.h"
#include "hazel/core/Application.h"
#include "hazel/core/Input.h"
#include "hazel/core/KeyCodes.h"
#include "hazel/core/Layer.h"
#include "hazel/core/Log.h"
#include "hazel/core/MouseCodes.h"
#include "hazel/core/Timestep.h"
#include "hazel/scene/Components.h"
#include "hazel/scene/Entity.h"
#include "hazel/scene/Scene.h"

// ---Renderer------------------------
#include "hazel/renderer/Buffer.h"
#include "hazel/renderer/Framebuffer.h"
#include "hazel/renderer/OrthographicCamera.h"
#include "hazel/renderer/OrthographicCameraController.h"
#include "hazel/renderer/RenderCommand.h"
#include "hazel/renderer/Renderer.h"
#include "hazel/renderer/Renderer2D.h"
#include "hazel/renderer/Shader.h"
#include "hazel/renderer/Texture.h"
#include "hazel/renderer/VertexArray.h"
// -----------------------------------

#include "hazel/debug/Instrumentor.h"