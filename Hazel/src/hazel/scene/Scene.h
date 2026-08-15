#pragma once

#include "Hazel/Core/Timestep.h"
#include "entt.hpp"

namespace hazel {

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

	/// @brief 在场景中创建一个新实体。
	/// @return 新实体的句柄，可用于挂载组件。
	entt::entity createEntity();

	// TEMP
	/// @brief 临时接口：直接访问内部的实体注册表。
	/// @return EnTT registry 的引用。
	entt::registry& reg() { return registry_; }

	/// @brief 更新场景中的所有实体（通常每帧调用一次）。
	/// @param ts 本帧的时间步长。
	void onUpdate(Timestep ts);

private:
	/// @brief 存储场景中所有实体及其组件的 EnTT 注册表。
	entt::registry registry_;
};

}  // namespace hazel