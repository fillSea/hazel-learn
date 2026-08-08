#pragma once

#include "OrthographicCamera.h"
#include "Texture.h"

namespace hazel {

/// @brief 2D 渲染器，提供静态的 2D 渲染接口。
///
/// 负责管理渲染所需的内部资源（如四边形顶点数组和着色器），
/// 并在场景开始后按帧提交 2D 图元绘制命令。
/// 所有成员均为静态方法，无需实例化即可调用。
class Renderer2D {
public:
	/// @brief 初始化 2D 渲染器，创建内部渲染资源。
	/// 必须在首次绘制前调用一次。
	static void init();

	/// @brief 关闭 2D 渲染器，释放内部渲染资源。
	/// 应在应用退出前调用一次。
	static void shutdown();

	/// @brief 开始一个场景的渲染。
	/// @param camera 当前使用的正交相机，用于计算视图-投影矩阵。
	/// 之后绘制的图元都基于该相机进行变换。
	static void beginScene(const OrthographicCamera& camera);

	/// @brief 结束当前场景的渲染。
	static void endScene();

	/// @brief 提交渲染命令，将缓冲区中的图元绘制到屏幕上。
	static void flush();

	// 图元绘制

	/// @brief 绘制一个纯色四边形（2D 位置重载）。
	/// @param position 四边形中心位置（2D 坐标，Z 取 0）。
	/// @param size 四边形的大小（宽、高）。
	/// @param color 四边形颜色（RGBA）。
	static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);

	/// @brief 绘制一个纯色四边形（3D 位置重载）。
	/// @param position 四边形中心位置（3D 坐标）。
	/// @param size 四边形的大小（宽、高）。
	/// @param color 四边形颜色（RGBA）。
	static void drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

	/// @brief 绘制一个带纹理的四边形（2D 位置重载）。
	/// @param position 四边形中心位置（2D 坐标，Z 取 0）。
	/// @param size 四边形的大小（宽、高）。
	/// @param texture 要贴到四边形上的 2D 纹理。
	static void drawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture,
	                     float tiling_factor = 1.0f, const glm::vec4& tint_color = glm::vec4(1.0f));

	/// @brief 绘制一个带纹理的四边形（3D 位置重载）。
	/// @param position 四边形中心位置（3D 坐标）。
	/// @param size 四边形的大小（宽、高）。
	/// @param texture 要贴到四边形上的 2D 纹理。
	static void drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture,
	                     float tiling_factor = 1.0f, const glm::vec4& tint_color = glm::vec4(1.0f));

	/// @brief 绘制一个带旋转的纯色四边形（2D 位置重载）。
	/// @param position 四边形中心位置（2D 坐标，Z 取 0）。
	/// @param size 四边形的大小（宽、高）。
	/// @param rotation 旋转角度（弧度，绕 Z 轴）。
	/// @param color 四边形颜色（RGBA）。
	static void drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation,
	                            const glm::vec4& color);

	/// @brief 绘制一个带旋转的纯色四边形（3D 位置重载）。
	/// @param position 四边形中心位置（3D 坐标）。
	/// @param size 四边形的大小（宽、高）。
	/// @param rotation 旋转角度（弧度，绕 Z 轴）。
	/// @param color 四边形颜色（RGBA）。
	static void drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation,
	                            const glm::vec4& color);

	/// @brief 绘制一个带旋转、带纹理的四边形（2D 位置重载）。
	/// @param position 四边形中心位置（2D 坐标，Z 取 0）。
	/// @param size 四边形的大小（宽、高）。
	/// @param rotation 旋转角度（弧度，绕 Z 轴）。
	/// @param texture 要贴到四边形上的 2D 纹理。
	/// @param tiling_factor 纹理平铺系数，大于 1 时纹理重复次数增加。
	/// @param tint_color 叠加到纹理上的颜色（RGBA，默认白色不改变原色）。
	static void drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation,
	                            const Ref<Texture2D>& texture, float tiling_factor = 1.0f,
	                            const glm::vec4& tint_color = glm::vec4(1.0f));

	/// @brief 绘制一个带旋转、带纹理的四边形（3D 位置重载）。
	/// @param position 四边形中心位置（3D 坐标）。
	/// @param size 四边形的大小（宽、高）。
	/// @param rotation 旋转角度（弧度，绕 Z 轴）。
	/// @param texture 要贴到四边形上的 2D 纹理。
	/// @param tiling_factor 纹理平铺系数，大于 1 时纹理重复次数增加。
	/// @param tint_color 叠加到纹理上的颜色（RGBA，默认白色不改变原色）。
	static void drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation,
	                            const Ref<Texture2D>& texture, float tiling_factor = 1.0f,
	                            const glm::vec4& tint_color = glm::vec4(1.0f));
};

}  // namespace hazel