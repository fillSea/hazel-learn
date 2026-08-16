#pragma once

#include "hazel/renderer/Camera.h"

namespace hazel {

class SceneCamera : public Camera {
public:
	SceneCamera();
	virtual ~SceneCamera() = default;

	void setOrthographic(float size, float near_clip, float far_clip);

	void setViewportSize(uint32_t width, uint32_t height);

	float getOrthographicSize() const { return orthographic_size_; }
	void setOrthographicSize(float size) {
		orthographic_size_ = size;
		recalculateProjection();
	}

private:
	void recalculateProjection();

private:
	float orthographic_size_ = 10.0f;
	float orthographic_near_ = -1.0f;
	float orthographic_far_ = 1.0f;

	float aspect_ratio_ = 0.0f;
};

}  // namespace hazel