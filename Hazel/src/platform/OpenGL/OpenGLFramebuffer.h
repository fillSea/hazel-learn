#pragma once

#include "hazel/renderer/Framebuffer.h"

namespace hazel {

// OpenGL 帧缓冲实现：封装 OpenGL 帧缓冲对象（FBO）及颜色/深度附件
class OpenGLFramebuffer : public Framebuffer {
public:
	explicit OpenGLFramebuffer(const FramebufferSpecification& spec);  // 根据规格创建并初始化帧缓冲
	virtual ~OpenGLFramebuffer();                                      // 销毁并释放 OpenGL 资源

	OpenGLFramebuffer(const OpenGLFramebuffer&) = delete;             // 禁止拷贝构造
	OpenGLFramebuffer& operator=(const OpenGLFramebuffer&) = delete;  // 禁止拷贝赋值
	OpenGLFramebuffer(OpenGLFramebuffer&&) = delete;                  // 禁止移动构造
	OpenGLFramebuffer& operator=(OpenGLFramebuffer&&) = delete;       // 禁止移动赋值

	void invalidate();  // （重新）创建/重建帧缓冲及其附件，用于初始化或规格变更后重建

	void bind() override;    // 绑定帧缓冲，使后续渲染输出到该缓冲
	void unbind() override;  // 解绑，恢复渲染到默认帧缓冲

	void resize(uint32_t width, uint32_t height) override;

	// 返回颜色附件（纹理）的渲染器 ID，供着色器采样使用
	uint32_t getColorAttachmentRendererID() const override { return color_attachment_; }

	// 返回该帧缓冲的规格信息
	const FramebufferSpecification& getSpecification() const override { return specification_; }

private:
	uint32_t renderer_id_{0};                 // OpenGL 帧缓冲对象（FBO）的 ID
	uint32_t color_attachment_{0};            // 颜色附件（颜色纹理）的渲染器 ID
	uint32_t depth_attachment_{0};            // 深度附件（深度缓冲）的渲染器 ID
	FramebufferSpecification specification_;  // 帧缓冲的规格配置
};

}  // namespace hazel