#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace hazel {

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
    : projection_matrix_(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), view_matrix_(1.0f) {
	view_matrix_projection_ = projection_matrix_ * view_matrix_;
}

void OrthographicCamera::recalculateViewMatrix() {
	// 构建相机变换：先平移到位置，再绕Z轴旋转
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position_) *
	                      glm::rotate(glm::mat4(1.0f), glm::radians(rotation_), glm::vec3(0, 0, 1));

	// 视图矩阵为相机变换的逆矩阵
	view_matrix_ = glm::inverse(transform);
	view_matrix_projection_ = projection_matrix_ * view_matrix_;
}

}  // namespace hazel