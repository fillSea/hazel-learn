#include "Entity.h"

namespace hazel {

Entity::Entity(entt::entity handle, Scene* scene) : entity_handle_(handle), scene_(scene) {}

}  // namespace hazel