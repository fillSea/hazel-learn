#pragma once

#include "hazel/renderer/Texture.h"

namespace hazel {

class HAZEL_API OpenGLTexture2D : public Texture2D {
public:
	explicit OpenGLTexture2D(const std::string& path);
	~OpenGLTexture2D() override;

	OpenGLTexture2D(const OpenGLTexture2D& other) = delete;
	OpenGLTexture2D& operator=(const OpenGLTexture2D& other) = delete;
	OpenGLTexture2D(OpenGLTexture2D&& other) = delete;
	OpenGLTexture2D& operator=(OpenGLTexture2D&& other) = delete;

	uint32_t getWidth() const override { return width_; }
	uint32_t getHeight() const override { return height_; }

	void bind(uint32_t slot = 0) const override;

private:
	std::string path_;
	uint32_t width_;
	uint32_t height_;
	uint32_t renderer_id_;
};

}  // namespace hazel