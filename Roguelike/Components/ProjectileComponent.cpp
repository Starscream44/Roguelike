#include "pch.h"
#include "ProjectileComponent.h"

#include "GameObject.h"
#include "GameWorld.h"
#include "RenderSystem.h"
#include "ResourceSystem.h"
#include "StatsComponent.h"
#include "TransformComponent.h"

#include <SFML/Graphics.hpp>
#include <cmath>

namespace Roguelike
{
namespace
{
// Projectile collision tuning. Obstacle radius is slightly larger than target radius so
// shots do not visually pass through wall edges before being destroyed.
const float ProjectileObstacleHitDistance = 36.0f;
const float ProjectileTargetHitDistance = 32.0f;
const float RadiansToDegrees = 180.0f / 3.14159265f;
}  // namespace

ProjectileComponent::ProjectileComponent(Engine::GameObject* gameObject) : Component(gameObject)
{
    transform = gameObject->GetComponent<Engine::TransformComponent>();
}

void ProjectileComponent::Update(float deltaTime)
{
    if (transform == nullptr)
    {
        return;
    }

    lifeTime -= deltaTime;

    if (lifeTime <= 0.f)
    {
        Engine::GameWorld::Instance()->DestroyGameObject(gameObject);
        return;
    }

    transform->MoveBy(direction.x * speed * deltaTime, direction.y * speed * deltaTime);

    if (CheckObstacles())
    {
        Engine::GameWorld::Instance()->DestroyGameObject(gameObject);
        return;
    }

    CheckTargets();
}

void ProjectileComponent::Render()
{
    if (transform == nullptr || Engine::GameWorld::Instance()->IsPaused())
    {
        return;
    }

    Engine::Vector2Df position = transform->GetWorldPosition();

    if (!textureKey.empty() && Engine::ResourceSystem::Instance()->HasTexture(textureKey))
    {
        const sf::Texture* texture = Engine::ResourceSystem::Instance()->GetTextureShared(textureKey);

        if (texture != nullptr)
        {
            sf::Sprite sprite(*texture);
            sf::Vector2u textureSize = texture->getSize();

            sprite.setOrigin(static_cast<float>(textureSize.x) * 0.5f,
                             static_cast<float>(textureSize.y) * 0.5f);
            sprite.setPosition(position.x, position.y);
            sprite.setScale(textureWidth / static_cast<float>(textureSize.x),
                            textureHeight / static_cast<float>(textureSize.y));
            // Projectile art points to the right in texture space, so rotate by movement angle.
            sprite.setRotation(std::atan2(direction.y, direction.x) * RadiansToDegrees);

            Engine::RenderSystem::Instance()->Render(sprite);
            return;
        }
    }

    sf::CircleShape shape(radius);
    shape.setOrigin(radius, radius);
    shape.setPosition(position.x, position.y);
    shape.setFillColor(sf::Color::Yellow);

    Engine::RenderSystem::Instance()->Render(shape);
}

void ProjectileComponent::SetDirection(const Engine::Vector2Df& newDirection)
{
    direction = newDirection;
}

void ProjectileComponent::SetDamage(float newDamage) { damage = newDamage; }

void ProjectileComponent::SetSpeed(float newSpeed) { speed = newSpeed; }

void ProjectileComponent::SetRadius(float newRadius) { radius = newRadius; }

void ProjectileComponent::SetLifeTime(float newLifeTime) { lifeTime = newLifeTime; }

void ProjectileComponent::SetTextureKey(const std::string& newTextureKey)
{
    textureKey = newTextureKey;
}

void ProjectileComponent::SetTextureSize(float width, float height)
{
    textureWidth = width;
    textureHeight = height;
}

void ProjectileComponent::SetTargets(const std::vector<Engine::GameObject*>& newTargets)
{
    targets = newTargets;
}

void ProjectileComponent::SetObstacles(const std::vector<Engine::GameObject*>& newObstacles)
{
    obstacles = newObstacles;
}

bool ProjectileComponent::CheckObstacles()
{
    Engine::Vector2Df projectilePosition = transform->GetWorldPosition();

    for (Engine::GameObject* obstacle : obstacles)
    {
        if (obstacle == nullptr)
        {
            continue;
        }

        if (!Engine::GameWorld::Instance()->IsGameObjectAlive(obstacle))
        {
            continue;
        }

        Engine::TransformComponent* obstacleTransform =
            obstacle->GetComponent<Engine::TransformComponent>();

        if (obstacleTransform == nullptr)
        {
            continue;
        }

        Engine::Vector2Df obstaclePosition = obstacleTransform->GetWorldPosition();

        Engine::Vector2Df difference = {obstaclePosition.x - projectilePosition.x,
                                        obstaclePosition.y - projectilePosition.y};

        if (difference.GetLength() <= ProjectileObstacleHitDistance)
        {
            return true;
        }
    }

    return false;
}

void ProjectileComponent::CheckTargets()
{
    for (Engine::GameObject* target : targets)
    {
        if (target == nullptr)
        {
            continue;
        }

        if (!Engine::GameWorld::Instance()->IsGameObjectAlive(target))
        {
            continue;
        }

        Engine::TransformComponent* targetTransform =
            target->GetComponent<Engine::TransformComponent>();

        Engine::StatsComponent* targetStats = target->GetComponent<Engine::StatsComponent>();

        if (targetTransform == nullptr || targetStats == nullptr)
        {
            continue;
        }

        if (targetStats->IsDead())
        {
            continue;
        }

        Engine::Vector2Df projectilePosition = transform->GetWorldPosition();
        Engine::Vector2Df targetPosition = targetTransform->GetWorldPosition();

        Engine::Vector2Df difference = {targetPosition.x - projectilePosition.x,
                                        targetPosition.y - projectilePosition.y};

        if (difference.GetLength() <= ProjectileTargetHitDistance)
        {
            targetStats->TakeDamage(damage);
            Engine::GameWorld::Instance()->DestroyGameObject(gameObject);
            return;
        }
    }
}
}  // namespace Roguelike
