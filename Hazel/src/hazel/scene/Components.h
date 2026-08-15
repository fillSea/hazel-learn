#pragma once

#include <glm/glm.hpp>

#include "hazel/renderer/Camera.h"

namespace hazel {

struct TagComponent {
	std::string tag;

	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	explicit TagComponent(const std::string& tag) : tag(tag) {}
};

struct TransformComponent {
	glm::mat4 transform{1.0f};

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	explicit TransformComponent(const glm::mat4& transform) : transform(transform) {}

	operator glm::mat4&() { return transform; }
	operator const glm::mat4&() const { return transform; }
};

struct SpriteRendererComponent {
	glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};

	SpriteRendererComponent() = default;
	SpriteRendererComponent(const SpriteRendererComponent&) = default;
	explicit SpriteRendererComponent(const glm::vec4& color) : color(color) {}
};

struct CameraComponent {
	Camera Camera;
	bool Primary = true;  // TODO: think about moving to Scene

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
	CameraComponent(const glm::mat4& projection) : Camera(projection) {}
};
}  // namespace hazel