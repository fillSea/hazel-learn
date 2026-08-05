#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace hazel {

//---------------------------------------- VertexBuffer ----------------------------------------
OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) {
	HZ_PROFILE_FUNCTION();

	glCreateBuffers(1, &renderer_id_);
	glBindBuffer(GL_ARRAY_BUFFER, renderer_id_);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
	HZ_PROFILE_FUNCTION();

	glDeleteBuffers(1, &renderer_id_);
}

void OpenGLVertexBuffer::bind() const {
	HZ_PROFILE_FUNCTION();

	glBindBuffer(GL_ARRAY_BUFFER, renderer_id_);
}

void OpenGLVertexBuffer::unBind() const {
	HZ_PROFILE_FUNCTION();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//---------------------------------------- IndexBuffer ----------------------------------------
OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : count_(count) {
	HZ_PROFILE_FUNCTION();

	glCreateBuffers(1, &renderer_id_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() {
	HZ_PROFILE_FUNCTION();

	glDeleteBuffers(1, &renderer_id_);
}

void OpenGLIndexBuffer::bind() const {
	HZ_PROFILE_FUNCTION();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_);
}

void OpenGLIndexBuffer::unBind() const {
	HZ_PROFILE_FUNCTION();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}  // namespace hazel