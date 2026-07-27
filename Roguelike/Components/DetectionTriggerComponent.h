#pragma once

#include "ColliderComponent.h"
#include "TransformComponent.h"

namespace Roguelike
{
class DetectionTriggerComponent : public Engine::ColliderComponent
{
   public:
    DetectionTriggerComponent(Engine::GameObject* gameObject);
    ~DetectionTriggerComponent();

    void Update(float deltaTime) override;
    void Render() override;

    void SetRadius(float newRadius);
    void SetShowDebug(bool value);

   private:
    Engine::TransformComponent* transform = nullptr;

    float radius = 180.f;
    bool showDebug = false;
};
}  // namespace Roguelike