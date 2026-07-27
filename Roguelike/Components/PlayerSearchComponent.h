#pragma once

#include "Component.h"
#include "GameObject.h"
#include "RigidbodyComponent.h"
#include "TransformComponent.h"
#include "Vector.h"

#include <vector>

namespace Engine
{
class AttackComponent;
}

namespace Roguelike
{
class PlayerSearchComponent : public Engine::Component
{
   public:
    PlayerSearchComponent(Engine::GameObject* gameObject);

    void Update(float deltaTime) override;
    void Render() override;

    void SetPlayer(Engine::GameObject* newPlayer);
    void SetPlayerDetected(bool detected);
    void SetSpeed(float newSpeed);

   private:
    void UpdatePath();
    void MoveByPath();

   private:
    Engine::GameObject* player = nullptr;

    Engine::TransformComponent* transform = nullptr;
    Engine::RigidbodyComponent* rigidbody = nullptr;
    Engine::AttackComponent* attack = nullptr;

    bool isPlayerDetected = false;

    float speed = 140.f;
    float stopDistance = 58.f;

    float attackCooldown = 1.f;
    float currentAttackCooldown = 0.f;

    float pathUpdateCooldown = 0.8f;
    float currentPathUpdateCooldown = 0.f;

    std::vector<Engine::Vector2Df> path;
    int currentPathIndex = 0;
};
}  // namespace Roguelike