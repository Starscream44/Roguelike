#pragma once

#include "ColliderComponent.h"

namespace Engine
{
class GameObject;
}

namespace Roguelike
{
class PickupComponent : public Engine::ColliderComponent
{
   public:
    explicit PickupComponent(Engine::GameObject* gameObject);
    ~PickupComponent() override;

   protected:
    bool IsCollected() const;

   private:
    void HandleTrigger(Engine::Trigger trigger);
    virtual bool Collect(Engine::GameObject* collectorObject) = 0;

   private:
    bool isCollected = false;
};
}  // namespace Roguelike
