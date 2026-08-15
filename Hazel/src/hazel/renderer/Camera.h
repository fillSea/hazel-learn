#pragma once

#include <glm/glm.hpp>

namespace hazel {

class Camera {
public:
	explicit Camera(const glm::mat4& projection) : projection_(projection) {}

	const glm::mat4& getProjection() const { return projection_; }

private:
	glm::mat4 projection_;
};

}  // namespace hazel