#include <Hazel.h>
#include <imgui.h>

class ExampleLayer : public hazel::Layer {
public:
	ExampleLayer() : Layer("Example"), camera_(-1.6f, 1.6f, -0.9f, 0.9f) {
		// 创建并绑定顶点数组对象 (VAO)，后续顶点属性配置都会记录在此 VAO 中
		vertex_array_.reset(hazel::VertexArray::create());

		// 三角形的 3 个顶点，每个顶点 3 个分量 (x, y, z)
		float vertices[3 * 7] = {-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f, 0.5f, -0.5f, 0.0f, 0.2f,
		                         0.3f,  0.8f,  1.0f, 0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f,  1.0f};

		// 创建并绑定顶点缓冲对象 (VBO)，用于存储顶点数据
		std::shared_ptr<hazel::VertexBuffer> vertex_buffer;
		vertex_buffer.reset(hazel::VertexBuffer::create(vertices, sizeof(vertices)));
		hazel::BufferLayout layout = {{hazel::ShaderDataType::Float3, "a_Position"},
		                              {hazel::ShaderDataType::Float4, "a_Color"}};
		vertex_buffer->setLayout(layout);
		vertex_array_->addVertexBuffer(vertex_buffer);

		// 索引数据：按顺序绘制 3 个顶点
		uint32_t indices[3] = {0, 1, 2};
		// 创建并绑定索引缓冲对象 (IBO/EBO)，用于索引绘制
		std::shared_ptr<hazel::IndexBuffer> index_buffer;
		index_buffer.reset(hazel::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
		vertex_array_->setIndexBuffer(index_buffer);

		// 着色器
		std::string vertex_src = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
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

		shader_ = std::make_shared<hazel::Shader>(vertex_src, fragment_src);

		square_va_.reset(hazel::VertexArray::create());
		float square_vertices[3 * 4] = {-0.75f, -0.75f, 0.0f, 0.75f,  -0.75f, 0.0f,
		                                0.75f,  0.75f,  0.0f, -0.75f, 0.75f,  0.0f};
		std::shared_ptr<hazel::VertexBuffer> square_vb;
		square_vb.reset(hazel::VertexBuffer::create(square_vertices, sizeof(square_vertices)));
		square_vb->setLayout({{hazel::ShaderDataType::Float3, "a_Position"}});
		square_va_->addVertexBuffer(square_vb);

		uint32_t square_indices[6] = {0, 1, 2, 2, 3, 0};
		std::shared_ptr<hazel::IndexBuffer> square_ib;
		square_ib.reset(hazel::IndexBuffer::create(square_indices, sizeof(square_indices) / sizeof(uint32_t)));
		square_va_->setIndexBuffer(square_ib);

		std::string blue_shader_vertex_src = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
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

		blue_shader_ = std::make_shared<hazel::Shader>(blue_shader_vertex_src, blue_shader_fragment_src);
	}

	void onUpdate(hazel::Timestep ts) override {
		if (hazel::Input::isKeyPressed(HZ_KEY_LEFT)) {
			camera_position_.x += camera_move_speed_ * ts;
		} else if (hazel::Input::isKeyPressed(HZ_KEY_RIGHT)) {
			camera_position_.x -= camera_move_speed_ * ts;
		}

		if (hazel::Input::isKeyPressed(HZ_KEY_UP)) {
			camera_position_.y -= camera_move_speed_ * ts;
		} else if (hazel::Input::isKeyPressed(HZ_KEY_DOWN)) {
			camera_position_.y += camera_move_speed_ * ts;
		}

		if (hazel::Input::isKeyPressed(HZ_KEY_A)) {
			camera_rotation_ += camera_rotation_speed_ * ts;
		}
		if (hazel::Input::isKeyPressed(HZ_KEY_D)) {
			camera_rotation_ -= camera_rotation_speed_ * ts;
		}

		hazel::RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 1});
		hazel::RenderCommand::clear();

		camera_.setPosition(camera_position_);
		camera_.setRotation(camera_rotation_);

		hazel::Renderer::beginScene(camera_);

		hazel::Renderer::submit(blue_shader_, square_va_);
		hazel::Renderer::submit(shader_, vertex_array_);

		hazel::Renderer::endScene();
	}

	void onEvent(hazel::Event& event) override {}

	void onImGuiRender() override {}

private:
	std::shared_ptr<hazel::Shader> shader_;
	std::shared_ptr<hazel::VertexArray> vertex_array_;

	std::shared_ptr<hazel::Shader> blue_shader_;
	std::shared_ptr<hazel::VertexArray> square_va_;

	hazel::OrthographicCamera camera_;
	glm::vec3 camera_position_{0.0f, 0.0f, 0.0f};
	float camera_move_speed_{5.0f};

	float camera_rotation_{0.0f};
	float camera_rotation_speed_{180.0f};
};

class Sandbox : public hazel::Application {
public:
	Sandbox() { pushLayer(new ExampleLayer()); }
	~Sandbox() override = default;

	Sandbox(const Sandbox&) = delete;
	Sandbox& operator=(const Sandbox&) = delete;
	Sandbox(Sandbox&&) = delete;
	Sandbox& operator=(Sandbox&&) = delete;
};

hazel::Application* hazel::createApplication() {
	return new Sandbox();
}