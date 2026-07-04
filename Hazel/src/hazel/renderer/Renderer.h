#pragma once
#include "OrthographicCamera.h"
#include "RendererAPI.h"
#include "Shader.h"
#include "hazel/Core.h"

namespace hazel {

/**
 * @brief 渲染器高层接口
 *
 * 提供场景级别的渲染生命周期管理（begin/end）与绘制提交接口，
 * 内部通过 RenderCommand 将指令分发到当前平台的 RendererAPI 实现。
 */
class HAZEL_API Renderer {
public:
	/**
	 * @brief 开始场景渲染
	 * 从相机获取视图投影矩阵并存储到场景数据中，供后续提交绘制调用使用。
	 * @param camera 当前场景使用的正交相机
	 */
	static void beginScene(OrthographicCamera& camera);

	/**
	 * @brief 结束场景渲染，完成后续处理
	 */
	static void endScene();

	/**
	 * @brief 提交绘制调用
	 * 绑定着色器并上传视图投影矩阵，绑定顶点数组，执行索引绘制。
	 * @param shader      着色器
	 * @param vertex_array 顶点数组
	 */
	static void submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertex_array);

	/**
	 * @brief 获取当前使用的图形 API 类型
	 * @return 当前 API 枚举值
	 */
	inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }

private:
	/// 当前场景的渲染数据
	struct SceneData {
		glm::mat4 view_projection_matrix;  ///< 视图投影组合矩阵
	};

	static SceneData* s_scene_data_;  ///< 场景数据单例指针
};

}  // namespace hazel