#include "ExampleLayer.h"

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "hazel/renderer/Shader.h"
#include "platform/OpenGL/OpenGLShader.h"

ExampleLayer::ExampleLayer() : Layer("Example"), camera_controller_(1280.0f / 720.0f) {
	// 创建并绑定顶点数组对象 (VAO)，后续顶点属性配置都会记录在此 VAO 中
	vertex_array_ = hazel::VertexArray::create();

	// 三角形的 3 个顶点，每个顶点 3 个分量 (x, y, z)
	float vertices[3 * 7] = {-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f, 0.5f, -0.5f, 0.0f, 0.2f,
	                         0.3f,  0.8f,  1.0f, 0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f,  1.0f};

	// 创建并绑定顶点缓冲对象 (VBO)，用于存储顶点数据
	hazel::Ref<hazel::VertexBuffer> vertex_buffer;
	vertex_buffer.reset(hazel::VertexBuffer::create(vertices, sizeof(vertices)));
	hazel::BufferLayout layout = {{hazel::ShaderDataType::Float3, "a_Position"},
	                              {hazel::ShaderDataType::Float4, "a_Color"}};
	vertex_buffer->setLayout(layout);
	vertex_array_->addVertexBuffer(vertex_buffer);

	// 索引数据：按顺序绘制 3 个顶点
	uint32_t indices[3] = {0, 1, 2};
	// 创建并绑定索引缓冲对象 (IBO/EBO)，用于索引绘制
	hazel::Ref<hazel::IndexBuffer> index_buffer;
	index_buffer.reset(hazel::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
	vertex_array_->setIndexBuffer(index_buffer);

	// 着色器
	std::string vertex_src = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
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

	shader_ = hazel::Shader::create("VertexPosColor", vertex_src, fragment_src);

	square_va_ = hazel::VertexArray::create();
	float square_vertices[5 * 4] = {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.0f,
	                                0.5f,  0.5f,  0.0f, 1.0f, 1.0f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f};
	hazel::Ref<hazel::VertexBuffer> square_vb;
	square_vb.reset(hazel::VertexBuffer::create(square_vertices, sizeof(square_vertices)));
	square_vb->setLayout(
	    {{hazel::ShaderDataType::Float3, "a_Position"}, {hazel::ShaderDataType::Float2, "a_TexCoord"}});
	square_va_->addVertexBuffer(square_vb);

	uint32_t square_indices[6] = {0, 1, 2, 2, 3, 0};
	hazel::Ref<hazel::IndexBuffer> square_ib;
	square_ib.reset(hazel::IndexBuffer::create(square_indices, sizeof(square_indices) / sizeof(uint32_t)));
	square_va_->setIndexBuffer(square_ib);

	std::string flat_color_shader_vertex_src = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

	std::string flat_color_shader_fragment_src = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			
			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";
	flat_color_shader_ =
	    hazel::Shader::create("FlatColor", flat_color_shader_vertex_src, flat_color_shader_fragment_src);

	auto texture_shader = shader_library_.load("assets/shaders/Texture.glsl");

	texture_ = hazel::Texture2D::create("assets/textures/Checkerboard.png");
	cherno_logo_texture_ = hazel::Texture2D::create("assets/textures/ChernoLogo.png");

	std::dynamic_pointer_cast<hazel::OpenGLShader>(texture_shader)->bind();
	std::dynamic_pointer_cast<hazel::OpenGLShader>(texture_shader)->uploadUniformInt("u_Texture", 0);
}

void ExampleLayer::onUpdate(hazel::Timestep ts) {
	camera_controller_.onUpdate(ts);

	hazel::RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 1});
	hazel::RenderCommand::clear();

	hazel::Renderer::beginScene(camera_controller_.getCamera());

	// hazel::Renderer::submit(shader_, vertex_array_);

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

	std::dynamic_pointer_cast<hazel::OpenGLShader>(flat_color_shader_)->bind();
	std::dynamic_pointer_cast<hazel::OpenGLShader>(flat_color_shader_)->uploadUniformFloat3("u_Color", square_color_);
	for (int y = 0; y < 20; y++) {
		for (int x = 0; x < 20; x++) {
			glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
			hazel::Renderer::submit(flat_color_shader_, square_va_, transform);
		}
	}
	auto texture_shader = shader_library_.get("Texture");

	texture_->bind();
	hazel::Renderer::submit(texture_shader, square_va_, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
	cherno_logo_texture_->bind();
	hazel::Renderer::submit(texture_shader, square_va_, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	hazel::Renderer::endScene();
}

void ExampleLayer::onEvent(hazel::Event& e) {
	camera_controller_.onEvent(e);
}

void ExampleLayer::onImGuiRender() {
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(square_color_));
	ImGui::End();
}