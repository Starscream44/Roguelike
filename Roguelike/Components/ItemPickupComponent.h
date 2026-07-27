#pragma once

#include "ColliderComponent.h"
#include "ItemData.h"
#include "PickupComponent.h"

namespace Engine
{
class TransformComponent;
}

namespace Roguelike
{
class ItemPickupComponent : public PickupComponent
{
   public:
    explicit ItemPickupComponent(Engine::GameObject* gameObject);

    void SetItem(const ItemStack& newItem);

    void Update(float deltaTime) override;
    void Render() override;

   private:
    bool Collect(Engine::GameObject* collectorObject) override;

   private:
    Engine::TransformComponent* transform = nullptr;
    ItemStack item;
};
}  // namespace Roguelike
