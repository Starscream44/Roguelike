#include "pch.h"
#include "RangedAttackComponent.h"

#include "GameConfig.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "ProjectileComponent.h"
#include "RenderSystem.h"
#include "StatsComponent.h"
#include "TransformComponent.h"
#include "Vector.h"

#include <SFML/Window/Mouse.hpp>

namespace Roguelike
{
RangedAttackComponent::RangedAttackComponent(Engine::GameObject* gameObject) : Component(gameObject)
{
    transform = gameObject->GetComponent<Engine::TransformComponent>();
}

void RangedAttackComponent::Update(float deltaTime)
{
    bool isLeftMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

    if (!isLeftMousePressed)
    {
        wasLeftMousePressed = false;
        return;
    }

    if (wasLeftMousePressed)
    {
        return;
    }

    wasLeftMousePressed = true;

    Shoot();
}

void RangedAttackComponent::Render() {}

void RangedAttackComponent::AddTarget(Engine::GameObject* target)
{
    if (target != nullptr)
    {
        targets.push_back(target);
    }
}

void RangedAttackComponent::SetTargets(const std::vector<Engine::GameObject*>& newTargets)
{
    targets = newTargets;
}

void RangedAttackComponent::SetObstacles(const std::vector<Engine::GameObject*>& newObstacles)
{
    obstacles = newObstacles;
}

void RangedAttackComponent::SetProjectileTextureKey(const std::string& newTextureKey)
{
    projectileTextureKey = newTextureKey;
}

void RangedAttackComponent::Shoot()
{
    if (transform == nullptr)
    {
        return;
    }

    sf::RenderWindow& window = Engine::RenderSystem::Instance()->GetMainWindow();

    sf::Vector2i mousePixelPosition = sf::Mouse::getPosition(window);

    sf::Vector2f mouseWorldPosition = window.mapPixelToCoords(mousePixelPosition);

    Engine::Vector2Df playerPosition = transform->GetWorldPosition();

    Engine::Vector2Df direction = {mouseWorldPosition.x - playerPosition.x,
                                   mouseWorldPosition.y - playerPosition.y};

    float length = direction.GetLength();

    if (length <= 0.01f)
    {
        // Ignore clicks exactly on the player to avoid a zero-length projectile direction.
        return;
    }

    direction.x /= length;
    direction.y /= length;

    Engine::GameObject* projectile = Engine::GameWorld::Instance()->CreateGameObject("Projectile");

    projectile->GetComponent<Engine::TransformComponent>()->SetWorldPosition(playerPosition.x,
                                                                             playerPosition.y);

    ProjectileComponent* projectileComponent = projectile->AddComponent<ProjectileComponent>();

    projectileComponent->SetDirection(direction);
    Engine::StatsComponent* stats = gameObject->GetComponent<Engine::StatsComponent>();
    // Attack potion modifies StatsComponent, so projectile damage is read at shot time.
    projectileComponent->SetDamage(stats != nullptr ? stats->GetAttackPower()
                                                     : GameConfig::PlayerAttackPower);
    projectileComponent->SetSpeed(GameConfig::ProjectileSpeed);
    projectileComponent->SetRadius(GameConfig::ProjectileRadius);
    projectileComponent->SetLifeTime(GameConfig::ProjectileLifeTime);
    projectileComponent->SetTextureKey(projectileTextureKey);
    projectileComponent->SetTextureSize(GameConfig::ProjectileTextureWidth,
                                        GameConfig::ProjectileTextureHeight);
    projectileComponent->SetTargets(targets);
    projectileComponent->SetObstacles(obstacles);
}
}  // namespace Roguelike
