#include "pch.h"
#include "PlayerMovementComponent.h"

#include "GameConfig.h"
#include "StatsComponent.h"

#include <algorithm>

namespace Roguelike
{
PlayerMovementComponent::PlayerMovementComponent(Engine::GameObject* gameObject)
    : Component(gameObject)
{
    input = gameObject->GetComponent<Engine::InputComponent>();
    rigidbody = gameObject->GetComponent<Engine::RigidbodyComponent>();
}

void PlayerMovementComponent::Update(float deltaTime)
{
    if (input == nullptr || rigidbody == nullptr)
    {
        return;
    }

    Engine::StatsComponent* stats = gameObject->GetComponent<Engine::StatsComponent>();

    if (stats != nullptr && stats->IsDead())
    {
        rigidbody->SetLinearVelocity({0.f, 0.f});
        return;
    }

    Engine::Vector2Df direction = {input->GetHorizontalAxis(), input->GetVerticalAxis()};

    float length = direction.GetLength();

    if (length > 0.01f)
    {
        // Normalize so diagonal movement is not faster than horizontal/vertical movement.
        direction.x /= length;
        direction.y /= length;
    }

    rigidbody->SetLinearVelocity(direction * speed);
}

void PlayerMovementComponent::Render() {}

void PlayerMovementComponent::SetSpeed(float newSpeed)
{
    // Speed potions can increase movement, but the cap prevents runaway saved values.
    speed = std::clamp(newSpeed, 0.0f, GameConfig::MaxPlayerMoveSpeed);
}

float PlayerMovementComponent::GetSpeed() const { return speed; }
}  // namespace Roguelike
