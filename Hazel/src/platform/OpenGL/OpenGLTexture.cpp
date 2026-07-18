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
	}
	width_ = width;
	height_ = height;

	glCreateTextures(GL_TEXTURE_2D, 1, &renderer_id_);
	glTextureStorage2D(renderer_id_, 1, GL_RGB8, width_, height_);

	glTextureParameteri(renderer_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(renderer_id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureSubImage2D(renderer_id_, 0, 0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D() {
	glDeleteTextures(1, &renderer_id_);
}

void OpenGLTexture2D::bind(uint32_t slot) const {
	glBindTextureUnit(slot, renderer_id_);
}
}  // namespace hazel
