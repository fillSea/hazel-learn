#include "Application.h"

#include <cstdint>
#include <memory>

#include "hazel/renderer/Buffer.h"
#include "hazel/renderer/RenderCommand.h"
#include "hazel/renderer/Renderer.h"
#include "hazel/renderer/VertexArray.h"

namespace hazel {
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

Application* Application::instance_ = nullptr;

Application::Application() {
	HZ_CORE_ASSERT(!instance_, "Application already exists!");
	instance_ = this;

	window_ = std::unique_ptr<Window>(Window::create());
	window_->setEventCallback(BIND_EVENT_FN(Application::onEvent));

	imGui_layer_ = new ImGuiLayer();
	pushOverlay(imGui_layer_);

	// 创建并绑定顶点数组对象 (VAO)，后续顶点属性配置都会记录在此 VAO 中
	vertex_array_.reset(VertexArray::create());

	// 三角形的 3 个顶点，每个顶点 3 个分量 (x, y, z)
	float vertices[3 * 7] = {-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f, 0.5f, -0.5f, 0.0f, 0.2f,
	                         0.3f,  0.8f,  1.0f, 0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f,  1.0f};

	// 创建并绑定顶点缓冲对象 (VBO)，用于存储顶点数据
	std::shared_ptr<VertexBuffer> vertex_buffer;
	vertex_buffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));
	BufferLayout layout = {{ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float4, "a_Color"}};
	vertex_buffer->setLayout(layout);
	vertex_array_->addVertexBuffer(vertex_buffer);

	// 索引数据：按顺序绘制 3 个顶点
	uint32_t indices[3] = {0, 1, 2};
	// 创建并绑定索引缓冲对象 (IBO/EBO)，用于索引绘制
	std::shared_ptr<IndexBuffer> index_buffer;
	index_buffer.reset(IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
	vertex_array_->setIndexBuffer(index_buffer);

	// 着色器
	std::string vertex_src = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

	std::string fragment_src = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

	shader_ = std::make_shared<Shader>(vertex_src, fragment_src);

	square_va_.reset(VertexArray::create());
	float square_vertices[3 * 4] = {-0.75f, -0.75f, 0.0f, 0.75f, -0.75f, 0.0f, 0.75f, 0.75f, 0.0f, -0.75f, 0.75f, 0.0f};
	std::shared_ptr<VertexBuffer> square_vb;
	square_vb.reset(VertexBuffer::create(square_vertices, sizeof(square_vertices)));
	square_vb->setLayout({{ShaderDataType::Float3, "a_Position"}});
	square_va_->addVertexBuffer(square_vb);

	uint32_t square_indices[6] = {0, 1, 2, 2, 3, 0};
	std::shared_ptr<IndexBuffer> square_ib;
	square_ib.reset(IndexBuffer::create(square_indices, sizeof(square_indices) / sizeof(uint32_t)));
	square_va_->setIndexBuffer(square_ib);

	std::string blue_shader_vertex_src = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

	std::string blue_shader_fragment_src = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

	blue_shader_ = std::make_shared<Shader>(blue_shader_vertex_src, blue_shader_fragment_src);
}

Application::~Application() = default;

void Application::pushLayer(Layer* layer) {
	layer_stack_.pushLayer(layer);
}

void Application::pushOverlay(Layer* overlay) {
	layer_stack_.pushOverlay(overlay);
}

void Application::onEvent(Event& e) {
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::onWindowClose));

	// 从后往前遍历，先处理上层的事件
	for (auto it = layer_stack_.end(); it != layer_stack_.begin();) {
		(*--it)->onEvent(e);
		// 如果事件被处理，就跳出循环
		if (e.isHandled()) {
			break;
		}
	}
}

void Application::run() {
	while (running_) {
		RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 1});
		RenderCommand::clear();

		Renderer::beginScene();

		blue_shader_->bind();
		Renderer::submit(square_va_);

		shader_->bind();
		Renderer::submit(vertex_array_);

		Renderer::endScene();

		for (Layer* layer : layer_stack_) {
			layer->onUpdate();
		}

		imGui_layer_->begin();
		for (Layer* layer : layer_stack_) {
			layer->onImGuiRender();
		}
		imGui_layer_->end();

		window_->onUpdate();
	}
}

bool Application::onWindowClose(WindowCloseEvent& e) {
	running_ = false;
	return true;
}
}  // namespace hazel