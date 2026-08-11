#pragma once

#include "hazel/core/Core.h"

namespace hazel {

// 帧缓冲规格：描述帧缓冲的尺寸、采样数等创建配置
struct FramebufferSpecification {
	uint32_t width;   // 帧缓冲宽度（像素）
	uint32_t height;  // 帧缓冲高度（像素）
	// FramebufferFormat Format =   // 帧缓冲的像素格式（暂未启用）
	uint32_t samples{1};  // 多重采样数量，默认为 1（即不启用多重采样）

	bool swap_chain_target{false};  // 是否为交换链目标（即默认帧缓冲，无需额外创建）
};

// 帧缓冲抽象基类：封装帧缓冲对象（如 OpenGL 的 FBO），用于离屏渲染
class Framebuffer {
public:
	virtual void bind() = 0;    // 绑定帧缓冲，使后续渲染输出到该缓冲
	virtual void unbind() = 0;  // 解绑帧缓冲，恢复渲染到默认帧缓冲

	// 获取颜色附件（纹理）的渲染器 ID，便于后续在着色器中采样该纹理
	virtual uint32_t getColorAttachmentRendererID() const = 0;
	// 获取该帧缓冲的规格信息
	virtual const FramebufferSpecification& getSpecification() const = 0;

	// 工厂方法：根据给定规格创建具体的帧缓冲实例
	static Ref<Framebuffer> create(const FramebufferSpecification& spec);
};

}  // namespace hazel