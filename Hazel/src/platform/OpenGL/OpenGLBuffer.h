#pragma once

#include "hazel/renderer/Buffer.h"

namespace hazel {

class OpenGLVertexBuffer : public VertexBuffer {
public:
	explicit OpenGLVertexBuffer(uint32_t size);
	OpenGLVertexBuffer(float* vertices, uint32_t size);
	~OpenGLVertexBuffer() override;

	OpenGLVertexBuffer(const OpenGLVertexBuffer&) = delete;
	OpenGLVertexBuffer& operator=(const OpenGLVertexBuffer&) = delete;
	OpenGLVertexBuffer(OpenGLVertexBuffer&&) = delete;
	OpenGLVertexBuffer& operator=(OpenGLVertexBuffer&&) = delete;

	void bind() const override;
	void unBind() const override;

	void setData(const void* data, uint32_t size) override;

	const BufferLayout& getLayout() const override { return layout_; }
	void setLayout(const BufferLayout& layout) override { layout_ = layout; }

private:
	uint32_t renderer_id_;
	BufferLayout layout_;
};

class OpenGLIndexBuffer : public IndexBuffer {
public:
	OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
	~OpenGLIndexBuffer() override;

	OpenGLIndexBuffer(const OpenGLIndexBuffer&) = delete;
	OpenGLIndexBuffer& operator=(const OpenGLIndexBuffer&) = delete;
	OpenGLIndexBuffer(OpenGLIndexBuffer&&) = delete;
	OpenGLIndexBuffer& operator=(OpenGLIndexBuffer&&) = delete;

	void bind() const override;
	void unBind() const override;

	[[nodiscard]] uint32_t getCount() const override { return count_; }

private:
	uint32_t renderer_id_;
	uint32_t count_;
};

}  // namespace hazel