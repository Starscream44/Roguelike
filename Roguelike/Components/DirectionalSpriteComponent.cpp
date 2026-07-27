#include "pch.h"
#include "DirectionalSpriteComponent.h"

#include "GameObject.h"
#include "ResourceSystem.h"
#include "RigidbodyComponent.h"
#include "SpriteRendererComponent.h"

#include <cmath>

namespace Roguelike
{
namespace
{
// Velocity below this threshold is treated as idle, preventing texture flicker from tiny motion.
const float MovementDirectionThreshold = 1.0f;
}  // namespace

DirectionalSpriteComponent::DirectionalSpriteComponent(Engine::GameObject* gameObject)
    : Component(gameObject)
{
    rigidbody = gameObject->GetComponent<Engine::RigidbodyComponent>();
    renderer = gameObject->GetComponent<Engine::SpriteRendererComponent>();
}

void DirectionalSpriteComponent::Update(float deltaTime)
{
    if (rigidbody == nullptr || renderer == nullptr)
    {
        return;
    }

    ApplyTexture(GetDirectionFromVelocity());
}

void DirectionalSpriteComponent::Render() {}

void DirectionalSpriteComponent::SetTextures(const std::string& defaultTexture,
                                             const std::string& downTexture,
                                             const std::string& rightTexture,
                                             const std::string& upTexture,
                                             const std::string& leftTexture)
{
    defaultTextureKey = defaultTexture;
    downTextureKey = downTexture;
    rightTextureKey = rightTexture;
    upTextureKey = upTexture;
    leftTextureKey = leftTexture;

    ApplyTexture(currentDirection);
}

DirectionalSpriteComponent::Direction DirectionalSpriteComponent::GetDirectionFromVelocity() const
{
    Engine::Vector2Df velocity = rigidbody->GetLinearVelocity();

    if (velocity.GetLength() < MovementDirectionThreshold)
    {
        return Direction::Idle;
    }

    if (std::abs(velocity.x) > std::abs(velocity.y))
    {
        // Dominant axis decides the sprite direction for diagonal movement.
        return velocity.x > 0.0f ? Direction::Right : Direction::Left;
    }

    return velocity.y > 0.0f ? Direction::Down : Direction::Up;
}

const std::string& DirectionalSpriteComponent::GetTextureKey(Direction direction) const
{
    switch (direction)
    {
        case Direction::Idle:
            return defaultTextureKey;
        case Direction::Down:
            return downTextureKey;
        case Direction::Right:
            return rightTextureKey;
        case Direction::Up:
            return upTextureKey;
        case Direction::Left:
        default:
            return leftTextureKey;
    }
}

void DirectionalSpriteComponent::ApplyTexture(Direction direction)
{
    if (renderer == nullptr)
    {
        return;
    }

    if (hasAppliedTexture && direction == currentDirection)
    {
        return;
    }

    const std::string& textureKey = GetTextureKey(direction);

    if (textureKey.empty() || !Engine::ResourceSystem::Instance()->HasTexture(textureKey))
    {
        return;
    }

    const sf::Texture* texture = Engine::ResourceSystem::Instance()->GetTextureShared(textureKey);

    if (texture == nullptr)
    {
        return;
    }

    renderer->SetTexture(*texture);
    currentDirection = direction;
    hasAppliedTexture = true;
}
}  // namespace Roguelike
