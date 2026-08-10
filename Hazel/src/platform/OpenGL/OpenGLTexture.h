#pragma once

#include <glad/glad.h>

#include "hazel/renderer/Texture.h"

namespace hazel {

class HAZEL_API OpenGLTexture2D : public Texture2D {
public:
	OpenGLTexture2D(uint32_t width, uint32_t height);
	explicit OpenGLTexture2D(const std::string& path);
	~OpenGLTexture2D() override;

	OpenGLTexture2D(const OpenGLTexture2D& other) = delete;
	OpenGLTexture2D& operator=(const OpenGLTexture2D& other) = delete;
	OpenGLTexture2D(OpenGLTexture2D&& other) = delete;
	OpenGLTexture2D& operator=(OpenGLTexture2D&& other) = delete;

	uint32_t getWidth() const override { return width_; }
	uint32_t getHeight() const override { return height_; }

	void setData(void* data, uint32_t size) override;

	void bind(uint32_t slot = 0) const override;

	uint32_t getRendererID() const override { return renderer_id_; }

	bool operator==(const Texture& other) const override {
		return renderer_id_ == (dynamic_cast<const OpenGLTexture2D&>(other)).renderer_id_;
	}

private:
	std::string path_;        ///< 纹理图片文件路径（以宽高创建时为空）
	uint32_t width_;          ///< 纹理宽度（像素）
	uint32_t height_;         ///< 纹理高度（像素）
	uint32_t renderer_id_;    ///< OpenGL 纹理对象 ID（GL_TEXTURE_2D 句柄）
	GLenum internal_format_;  ///< 纹理内部存储格式（如 GL_RGBA8）
	GLenum data_format_;      ///< 上传像素数据的格式（如 GL_RGBA）
};

}  // namespace hazel