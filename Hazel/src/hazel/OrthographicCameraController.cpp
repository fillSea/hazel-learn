#include "OrthographicCameraController.h"

#include "hazel/Input.h"
#include "hazel/KeyCodes.h"

namespace hazel {

OrthographicCameraController::OrthographicCameraController(float aspect_ratio, bool rotation)
    : aspect_ratio_(aspect_ratio)
    , camera_(-aspect_ratio * zoom_level_, aspect_ratio * zoom_level_, -zoom_level_, zoom_level_)
    , rotation_(rotation) {}

void OrthographicCameraController::onUpdate(Timestep ts) {
	if (Input::isKeyPressed(HZ_KEY_A)) {
		camera_position_.x -= camera_translation_speed_ * ts;
	} else if (Input::isKeyPressed(HZ_KEY_D)) {
		camera_position_.x += camera_translation_speed_ * ts;
	}

	if (Input::isKeyPressed(HZ_KEY_W)) {
		camera_position_.y += camera_translation_speed_ * ts;
	} else if (Input::isKeyPressed(HZ_KEY_S)) {
		camera_position_.y -= camera_translation_speed_ * ts;
	}

	if (rotation_) {
		if (Input::isKeyPressed(HZ_KEY_Q)) {
			camera_rotation_ += camera_rotation_speed_ * ts;
		}
		if (Input::isKeyPressed(HZ_KEY_E)) {
			camera_rotation_ -= camera_rotation_speed_ * ts;
		}

		camera_.setRotation(camera_rotation_);
	}

	camera_.setPosition(camera_position_);

	camera_translation_speed_ = zoom_level_;
}

void OrthographicCameraController::onEvent(Event& e) {
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::onMouseScrolled));
	dispatcher.dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::onWindowResized));
}

bool OrthographicCameraController::onMouseScrolled(MouseScrolledEvent& e) {
	zoom_level_ -= e.getYOffset() * 0.25f;
	zoom_level_ = std::max(zoom_level_, 0.25f);
	camera_.setProjection(-aspect_ratio_ * zoom_level_, aspect_ratio_ * zoom_level_, -zoom_level_, zoom_level_);
	return false;
}

bool OrthographicCameraController::onWindowResized(WindowResizeEvent& e) {
	aspect_ratio_ = static_cast<float>(e.getWidth()) / static_cast<float>(e.getHeight());
	camera_.setProjection(-aspect_ratio_ * zoom_level_, aspect_ratio_ * zoom_level_, -zoom_level_, zoom_level_);
	return false;
}

}  // namespace hazel