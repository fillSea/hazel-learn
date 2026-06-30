#pragma once

#include "hazel/renderer/VertexArray.h"

namespace hazel {

class OpenGLVertexArray : public VertexArray {
public:
	OpenGLVertexArray();
	~OpenGLVertexArray() override;

	OpenGLVertexArray(const OpenGLVertexArray&) = delete;
	OpenGLVertexArray& operator=(const OpenGLVertexArray&) = delete;
	OpenGLVertexArray(OpenGLVertexArray&&) = delete;
	OpenGLVertexArray& operator=(OpenGLVertexArray&&) = delete;

	void bind() const override;
	void unBind() const override;

	void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertex_buffer) override;
	void setIndexBuffer(const std::shared_ptr<IndexBuffer>& index_buffer) override;

	[[nodiscard]] const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const override {
		return vertex_buffers_;
	}
	[[nodiscard]] const std::shared_ptr<IndexBuffer>& getIndexBuffer() const override { return index_buffer_; }

private:
	uint32_t renderer_id_;
	std::vector<std::shared_ptr<VertexBuffer>> vertex_buffers_;
	std::shared_ptr<IndexBuffer> index_buffer_;
};

}  // namespace hazel