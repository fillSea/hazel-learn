#pragma once

#include "Scene.h"
#include "entt.hpp"

namespace hazel {

class Entity {
public:
	Entity() = default;
	Entity(entt::entity handle, Scene* scene);
	Entity(const Entity& other) = default;

	template <typename T, typename... Args>
	T& addComponent(Args&&... args) {
		HZ_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!");
		return scene_->registry_.emplace<T>(entity_handle_, std::forward<Args>(args)...);
	}

	template <typename T>
	T& getComponent() {
		HZ_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
		return scene_->registry_.get<T>(entity_handle_);
	}

	template <typename T>
	bool hasComponent() {
		return scene_->registry_.has<T>(entity_handle_);
	}

	template <typename T>
	void removeComponent() {
		HZ_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
		scene_->registry_.remove<T>(entity_handle_);
	}

	operator bool() const { return entity_handle_ != entt::null; }

private:
	entt::entity entity_handle_{entt::null};
	Scene* scene_{nullptr};
};

}  // namespace hazel