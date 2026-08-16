#include "Scene.h"

#include <glm/glm.hpp>

#include "Components.h"
#include "Entity.h"
#include "hazel/renderer/Renderer2D.h"

namespace hazel {

static void doMath(const glm::mat4& transform) {}

static void onTransformConstruct(entt::registry& registry, entt::entity entity) {}

Scene::Scene() {
#if ENTT_EXAMPLE_CODE
	entt::entity entity = m_Registry.create();
	m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

	m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();

	if (m_Registry.has<TransformComponent>(entity)) {
		TransformComponent& transform = m_Registry.get<TransformComponent>(entity);
	}

	auto view = m_Registry.view<TransformComponent>();
	for (auto entity : view) {
		TransformComponent& transform = view.get<TransformComponent>(entity);
	}

	auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
	for (auto entity : group) {
		auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
	}
#endif
}

Scene::~Scene() = default;

Entity Scene::createEntity(const std::string& name) {
	Entity entity = {registry_.create(), this};
	entity.addComponent<TransformComponent>();
	auto& tag = entity.addComponent<TagComponent>();
	tag.tag = name.empty() ? "Entity" : name;
	return entity;
}

void Scene::onUpdate(Timestep ts) {
	Camera* main_camera = nullptr;
	glm::mat4* camera_transform = nullptr;
	{
		auto group = registry_.view<TransformComponent, CameraComponent>();
		for (auto entity : group) {
			auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

			if (camera.primary) {
				main_camera = &camera.camera;
				camera_transform = &transform.transform;
				break;
			}
		}
	}

	if (main_camera) {
		Renderer2D::beginScene(*main_camera, *camera_transform);

		auto group = registry_.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group) {
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::drawQuad(transform, sprite.color);
		}

		Renderer2D::endScene();
	}
}

void Scene::onViewportResize(uint32_t width, uint32_t height) {
	viewport_width_ = width;
	viewport_height_ = height;

	// Resize our non-FixedAspectRatio cameras
	auto view = registry_.view<CameraComponent>();
	for (auto entity : view) {
		auto& camera_component = view.get<CameraComponent>(entity);
		if (!camera_component.fixed_aspect_ratio) {
			camera_component.camera.setViewportSize(width, height);
		}
	}
}

}  // namespace hazel