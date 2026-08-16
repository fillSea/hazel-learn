#pragma once

#include <glm/glm.hpp>

#include "SceneCamera.h"

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
	SceneCamera camera;
	bool primary = true;  // TODO: think about moving to Scene
	bool fixed_aspect_ratio = false;

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
};
}  // namespace hazel