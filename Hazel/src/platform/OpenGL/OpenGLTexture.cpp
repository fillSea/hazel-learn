#include "OpenGLTexture.h"

#include <glad/glad.h>

#include "stb_image.h"

namespace hazel {

OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : path_(path) {
	int width, height, channels;
	HZ_CORE_INFO("Loading texture: {0}", path_);
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = stbi_load(path_.c_str(), &width, &height, &channels, 0);
	if (!data) {
		HZ_CORE_ERROR("Texture load failed: {0}. stb_image: {1}", path_, stbi_failure_reason());
		return;
	}
	width_ = width;
	height_ = height;

	GLenum internal_format = 0, data_format = 0;
	if (channels == 4) {
		internal_format = GL_RGBA8;
		data_format = GL_RGBA;
	} else if (channels == 3) {
		internal_format = GL_RGB8;
		data_format = GL_RGB;
	}

	HZ_CORE_ASSERT(internal_format & data_format, "Format not supported!");

	glCreateTextures(GL_TEXTURE_2D, 1, &renderer_id_);
	glTextureStorage2D(renderer_id_, 1, internal_format, width_, height_);

	glTextureParameteri(renderer_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(renderer_id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(renderer_id_, 0, 0, 0, width_, height_, data_format, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D() {
	glDeleteTextures(1, &renderer_id_);
}

void OpenGLTexture2D::bind(uint32_t slot) const {
	glBindTextureUnit(slot, renderer_id_);
}
}  // namespace hazel
