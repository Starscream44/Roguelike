// @file SpriteColliderComponent.h

#pragma once

#include "ColliderComponent.h"
#include "SpriteRendererComponent.h"
#include "RenderSystem.h"

#include <SFML/Graphics/Sprite.hpp>

namespace Engine
{
class SpriteColliderComponent : public ColliderComponent
{
   public:
    SpriteColliderComponent(GameObject* gameObject);
    ~SpriteColliderComponent();

    void Update(float deltaTime) override;
    void Render() override;

    void SetShowDebug(bool value);

   private:
    SpriteRendererComponent* spriteRenderer = nullptr;
    const sf::Sprite* sprite = nullptr;
    bool showDebug = false;
};
}  // namespace Engine
