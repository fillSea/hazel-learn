#pragma once

#include <glm/glm.hpp>

#include "hazel/core/Core.h"

namespace hazel {

/**
 * @brief 正交相机类
 * 提供无透视畸变的2D视角，常用于2D游戏或UI渲染。
 * 内部维护投影矩阵、视图矩阵及其组合矩阵，
 * 当位置或旋转发生变化时自动重新计算视图矩阵。
 */
class HAZEL_API OrthographicCamera {
public:
	/**
	 * @brief 构造正交相机
	 * 使用 glm::ortho 创建正交投影矩阵，视图矩阵初始化为单位矩阵，
	 * 随后计算投影×视图组合矩阵。
	 * @param left   视口左边界
	 * @param right  视口右边界
	 * @param bottom 视口下边界
	 * @param top    视口上边界
	 */
	OrthographicCamera(float left, float right, float bottom, float top);

	/** @brief 获取相机位置 */
	[[nodiscard]] const glm::vec3& getPosition() const { return position_; }
	/** @brief 设置相机位置，并重新计算视图矩阵 */
	void setPosition(const glm::vec3& position) {
		position_ = position;
		recalculateViewMatrix();
	}

	/** @brief 获取相机旋转角度（度） */
	[[nodiscard]] float getRotation() const { return rotation_; }
	/** @brief 设置相机旋转角度（度），并重新计算视图矩阵 */
	void setRotation(float rotation) {
		rotation_ = rotation;
		recalculateViewMatrix();
	}

	/**
	 * @brief 设置正交投影矩阵
	 * 使用 glm::ortho 重新构造投影矩阵，并更新投影×视图组合矩阵。
	 * @param left   视口左边界
	 * @param right  视口右边界
	 * @param bottom 视口下边界
	 * @param top    视口上边界
	 */
	void setProjection(float left, float right, float bottom, float top);

	/** @brief 获取投影矩阵 */
	[[nodiscard]] const glm::mat4& getProjectionMatrix() const { return projection_matrix_; }
	/** @brief 获取视图矩阵 */
	[[nodiscard]] const glm::mat4& getViewMatrix() const { return view_matrix_; }
	/** @brief 获取投影×视图组合矩阵 */
	[[nodiscard]] const glm::mat4& getViewProjectionMatrix() const { return view_matrix_projection_; }

private:
	/**
	 * @brief 根据当前位置和旋转重新计算视图矩阵及组合矩阵
	 * 先根据位置和平移构建相机变换矩阵（平移+绕Z轴旋转），
	 * 再对其求逆得到视图矩阵，最后更新投影×视图组合矩阵。
	 */
	void recalculateViewMatrix();

private:
	glm::mat4 projection_matrix_;       ///< 投影矩阵
	glm::mat4 view_matrix_;             ///< 视图矩阵
	glm::mat4 view_matrix_projection_;  ///< 投影×视图组合矩阵

	glm::vec3 position_{0.0f, 0.0f, 0.0f};  ///< 相机世界位置
	float rotation_{0.0f};                  ///< 相机绕Z轴旋转角度（度）
};

}  // namespace hazel