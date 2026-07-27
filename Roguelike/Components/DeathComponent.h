#pragma once

#include "Component.h"

namespace Engine
{
class StatsComponent;

class DeathComponent : public Component
{
   public:
    DeathComponent(GameObject* gameObject);

    void Update(float deltaTime) override;
    void Render() override;

   private:
    StatsComponent* stats = nullptr;
    bool isMarkedToDestroy = false;
};
}  // namespace Engine