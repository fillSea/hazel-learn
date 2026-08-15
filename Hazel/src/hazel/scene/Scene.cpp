#include "Scene.h"

#include <glm/glm.hpp>

#include "Components.h"
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

entt::entity Scene::createEntity() {
	return registry_.create();
}

void Scene::onUpdate(Timestep ts) {
	auto group = registry_.group<TransformComponent>(entt::get<SpriteRendererComponent>);
	for (auto entity : group) {
		auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

		Renderer2D::drawQuad(transform, sprite.color);
	}
}

}  // namespace hazel