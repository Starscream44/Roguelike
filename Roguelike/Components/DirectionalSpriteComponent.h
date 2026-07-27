#pragma once

#include "Component.h"

#include <string>

namespace Engine
{
class RigidbodyComponent;
class SpriteRendererComponent;
}

namespace Roguelike
{
// Switches an object's sprite texture according to its movement direction.
class DirectionalSpriteComponent : public Engine::Component
{
   public:
    explicit DirectionalSpriteComponent(Engine::GameObject* gameObject);

    void Update(float deltaTime) override;
    void Render() override;

    void SetTextures(const std::string& defaultTextureKey,
                     const std::string& downTextureKey,
                     const std::string& rightTextureKey,
                     const std::string& upTextureKey,
                     const std::string& leftTextureKey);

   private:
    enum class Direction
    {
        Down,
        Right,
        Up,
        Left,
        Idle
    };

    Direction GetDirectionFromVelocity() const;
    const std::string& GetTextureKey(Direction direction) const;
    void ApplyTexture(Direction direction);

   private:
    Engine::RigidbodyComponent* rigidbody = nullptr;
    Engine::SpriteRendererComponent* renderer = nullptr;

    std::string defaultTextureKey;
    std::string downTextureKey;
    std::string rightTextureKey;
    std::string upTextureKey;
    std::string leftTextureKey;

    Direction currentDirection = Direction::Idle;
    bool hasAppliedTexture = false;
};
}  // namespace Roguelike
