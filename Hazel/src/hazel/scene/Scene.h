#pragma once

#include "Hazel/Core/Timestep.h"
#include "entt.hpp"

namespace hazel {
class Entity;

/// @brief 场景类，管理场景中的实体及其实体上的组件。
///
/// 内部基于 EnTT 的 registry 存储实体和组件，
/// 负责实体的创建、组件的挂载以及场景的逐帧更新。
class Scene {
public:
	/// @brief 构造一个空场景。
	Scene();

	/// @brief 析构场景，释放所有实体和组件。
	~Scene();

	Entity createEntity(const std::string& name = std::string());

	/// @brief 更新场景中的所有实体（通常每帧调用一次）。
	/// @param ts 本帧的时间步长。
	void onUpdate(Timestep ts);

	void onViewportResize(uint32_t width, uint32_t height);

private:
	friend class Entity;

	/// @brief 存储场景中所有实体及其组件的 EnTT 注册表。
	entt::registry registry_;

	uint32_t viewport_width_ = 0;
	uint32_t viewport_height_ = 0;
};

}  // namespace hazel