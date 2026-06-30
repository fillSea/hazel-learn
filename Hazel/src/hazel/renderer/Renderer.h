#pragma once
#include "RendererAPI.h"

namespace hazel {

/**
 * @brief 渲染器高层接口
 *
 * 提供场景级别的渲染生命周期管理（begin/end）与绘制提交接口，
 * 内部通过 RenderCommand 将指令分发到当前平台的 RendererAPI 实现。
 */
class Renderer {
public:
	/**
	 * @brief 开始场景渲染，准备渲染上下文
	 */
	static void beginScene();

	/**
	 * @brief 结束场景渲染，完成后续处理
	 */
	static void endScene();

	/**
	 * @brief 提交绘制请求
	 * @param vertex_array 要绘制的顶点数组
	 */
	static void submit(const std::shared_ptr<VertexArray>& vertex_array);

	/**
	 * @brief 获取当前使用的图形 API 类型
	 * @return 当前 API 枚举值
	 */
	inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
};

}  // namespace hazel