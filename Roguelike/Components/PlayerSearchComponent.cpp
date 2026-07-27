#include "pch.h"
#include "PlayerSearchComponent.h"

#include "AttackComponent.h"
#include "GameWorld.h"
#include "MazeNavigation.h"
#include "StatsComponent.h"

#include <algorithm>

namespace Roguelike
{
namespace
{
// Enemy path-following tuning. Reach distance controls when an enemy advances to the next
// path node; min distance prevents tiny velocity jitter when already on the node.
const float PathNodeReachDistance = 12.0f;
const float MinMoveDistance = 0.01f;
}  // namespace

PlayerSearchComponent::PlayerSearchComponent(Engine::GameObject* gameObject) : Component(gameObject)
{
    transform = gameObject->GetComponent<Engine::TransformComponent>();
    rigidbody = gameObject->GetComponent<Engine::RigidbodyComponent>();
    attack = gameObject->GetComponent<Engine::AttackComponent>();
}

void PlayerSearchComponent::Update(float deltaTime)
{
    if (currentAttackCooldown > 0.f)
    {
        currentAttackCooldown = std::max(0.0f, currentAttackCooldown - deltaTime);
    }

    if (currentPathUpdateCooldown > 0.f)
    {
        currentPathUpdateCooldown = std::max(0.0f, currentPathUpdateCooldown - deltaTime);
    }

    if (rigidbody == nullptr)
    {
        return;
    }

    if (player == nullptr || !Engine::GameWorld::Instance()->IsGameObjectAlive(player))
    {
        rigidbody->SetLinearVelocity({0.f, 0.f});
        player = nullptr;
        isPlayerDetected = false;
        path.clear();
        currentPathIndex = 0;
        return;
    }

    Engine::StatsComponent* selfStats = gameObject->GetComponent<Engine::StatsComponent>();

    if (selfStats != nullptr && selfStats->IsDead())
    {
        rigidbody->SetLinearVelocity({0.f, 0.f});
        path.clear();
        currentPathIndex = 0;
        return;
    }

    Engine::StatsComponent* playerStats = player->GetComponent<Engine::StatsComponent>();

    if (playerStats != nullptr && playerStats->IsDead())
    {
        rigidbody->SetLinearVelocity({0.f, 0.f});
        isPlayerDetected = false;
        path.clear();
        currentPathIndex = 0;
        return;
    }

    if (!isPlayerDetected || transform == nullptr)
    {
        rigidbody->SetLinearVelocity({0.f, 0.f});
        return;
    }

    Engine::TransformComponent* playerTransform =
        player->GetComponent<Engine::TransformComponent>();

    if (playerTransform == nullptr)
    {
        rigidbody->SetLinearVelocity({0.f, 0.f});
        player = nullptr;
        isPlayerDetected = false;
        path.clear();
        currentPathIndex = 0;
        return;
    }

    Engine::Vector2Df playerPosition = playerTransform->GetWorldPosition();

    Engine::Vector2Df enemyPosition = transform->GetWorldPosition();

    Engine::Vector2Df directionToPlayer = {playerPosition.x - enemyPosition.x,
                                           playerPosition.y - enemyPosition.y};

    float distanceToPlayer = directionToPlayer.GetLength();

    if (distanceToPlayer < stopDistance)
    {
        rigidbody->SetLinearVelocity({0.f, 0.f});

        if (attack != nullptr && currentAttackCooldown <= 0.f)
        {
            attack->Attack(player);
            currentAttackCooldown = attackCooldown;
        }

        return;
    }

    if (currentPathUpdateCooldown <= 0.f)
    {
        // Rebuilding the path on a cooldown keeps enemies responsive without doing A* every frame.
        UpdatePath();
        currentPathUpdateCooldown = pathUpdateCooldown;
    }

    MoveByPath();
}

void PlayerSearchComponent::Render() {}

void PlayerSearchComponent::SetPlayer(Engine::GameObject* newPlayer) { player = newPlayer; }

void PlayerSearchComponent::SetSpeed(float newSpeed) { speed = newSpeed; }

void PlayerSearchComponent::SetPlayerDetected(bool detected)
{
    if (isPlayerDetected == detected)
    {
        return;
    }

    isPlayerDetected = detected;

    if (!isPlayerDetected)
    {
        path.clear();
        currentPathIndex = 0;
        currentPathUpdateCooldown = 0.0f;
        return;
    }

    // Detection should feel responsive, so the first path is built immediately.
    currentPathUpdateCooldown = 0.0f;
}

void PlayerSearchComponent::UpdatePath()
{
    if (player == nullptr || transform == nullptr)
    {
        return;
    }

    Engine::TransformComponent* playerTransform =
        player->GetComponent<Engine::TransformComponent>();

    if (playerTransform == nullptr)
    {
        return;
    }

    path = MazeNavigation::Instance()->FindPath(transform->GetWorldPosition(),
                                                playerTransform->GetWorldPosition());

    currentPathIndex = 0;
}

void PlayerSearchComponent::MoveByPath()
{
    if (path.empty() || currentPathIndex >= static_cast<int>(path.size()))
    {
        rigidbody->SetLinearVelocity({0.f, 0.f});
        return;
    }

    Engine::Vector2Df enemyPosition = transform->GetWorldPosition();

    Engine::Vector2Df targetPosition = path[currentPathIndex];

    Engine::Vector2Df direction = {targetPosition.x - enemyPosition.x,
                                   targetPosition.y - enemyPosition.y};

    float distance = direction.GetLength();

    if (distance < PathNodeReachDistance)
    {
        // Step to the next maze node once close enough; exact equality is too fragile with physics.
        currentPathIndex++;
        return;
    }

    if (distance <= MinMoveDistance)
    {
        rigidbody->SetLinearVelocity({0.f, 0.f});
        return;
    }

    direction.x /= distance;
    direction.y /= distance;

    rigidbody->SetLinearVelocity(direction * speed);
}
}  // namespace Roguelike
