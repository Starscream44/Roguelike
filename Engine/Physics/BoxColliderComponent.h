// @file BoxColliderComponent.h

#pragma once

#include "ColliderComponent.h"
#include "TransformComponent.h"

namespace Engine
{
class BoxColliderComponent : public ColliderComponent
{
   public:
    BoxColliderComponent(GameObject* gameObject);
    ~BoxColliderComponent();

    void Update(float deltaTime) override;
    void Render() override;

    void SetSize(float width, float height);
    void SetShowDebug(bool value);

   private:
    TransformComponent* transform = nullptr;
    Vector2Df size = {1.0f, 1.0f};
    bool showDebug = false;
};
}  // namespace Engine
