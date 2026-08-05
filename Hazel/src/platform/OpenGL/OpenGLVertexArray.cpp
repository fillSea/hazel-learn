#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace hazel {

static GLenum shaderDataTypeToOpenGLBaseType(ShaderDataType type) {
	HZ_PROFILE_FUNCTION();

	switch (type) {
		case hazel::ShaderDataType::Float:
			return GL_FLOAT;
		case hazel::ShaderDataType::Float2:
			return GL_FLOAT;
		case hazel::ShaderDataType::Float3:
			return GL_FLOAT;
		case hazel::ShaderDataType::Float4:
			return GL_FLOAT;
		case hazel::ShaderDataType::Mat3:
			return GL_FLOAT;
		case hazel::ShaderDataType::Mat4:
			return GL_FLOAT;
		case hazel::ShaderDataType::Int:
			return GL_INT;
		case hazel::ShaderDataType::Int2:
			return GL_INT;
		case hazel::ShaderDataType::Int3:
			return GL_INT;
		case hazel::ShaderDataType::Int4:
			return GL_INT;
		case hazel::ShaderDataType::Bool:
			return GL_BOOL;
		case ShaderDataType::None:
			break;
	}

	HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
	return 0;
}

OpenGLVertexArray::OpenGLVertexArray() {
	HZ_PROFILE_FUNCTION();

	glCreateVertexArrays(1, &renderer_id_);
}

OpenGLVertexArray::~OpenGLVertexArray() {
	HZ_PROFILE_FUNCTION();

	glDeleteVertexArrays(1, &renderer_id_);
}

void OpenGLVertexArray::bind() const {
	HZ_PROFILE_FUNCTION();

	glBindVertexArray(renderer_id_);
}

void OpenGLVertexArray::unBind() const {
	HZ_PROFILE_FUNCTION();

	glBindVertexArray(0);
}

void OpenGLVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertex_buffer) {
	HZ_PROFILE_FUNCTION();

	HZ_CORE_ASSERT(vertex_buffer->getLayout().getElements().size(), "Vertex Buffer has no layout!");

	glBindVertexArray(renderer_id_);
	vertex_buffer->bind();

	uint32_t index = 0;
	const auto& layout = vertex_buffer->getLayout();
	for (const auto& element : layout) {
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, element.getComponentCount(), shaderDataTypeToOpenGLBaseType(element.type),
		                      element.normalized ? GL_TRUE : GL_FALSE, layout.getStride(),
		                      reinterpret_cast<const void*>(static_cast<uintptr_t>(element.offset)));
		index++;
	}

	vertex_buffers_.push_back(vertex_buffer);
}

void OpenGLVertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& index_buffer) {
	HZ_PROFILE_FUNCTION();

	glBindVertexArray(renderer_id_);
	index_buffer->bind();

	index_buffer_ = index_buffer;
}

}  // namespace hazel