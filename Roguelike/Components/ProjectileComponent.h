#pragma once

#include "Component.h"
#include "Vector.h"

#include <string>
#include <vector>

namespace Engine
{
class GameObject;
class TransformComponent;
}  // namespace Engine

namespace Roguelike
{
// Moves projectile forward, applies damage to enemies and destroys it on walls.
class ProjectileComponent : public Engine::Component
{
   public:
    ProjectileComponent(Engine::GameObject* gameObject);

    void Update(float deltaTime) override;
    void Render() override;

    void SetDirection(const Engine::Vector2Df& newDirection);
    void SetDamage(float newDamage);
    void SetSpeed(float newSpeed);
    void SetRadius(float newRadius);
    void SetLifeTime(float newLifeTime);
    void SetTextureKey(const std::string& newTextureKey);
    void SetTextureSize(float width, float height);

    void SetTargets(const std::vector<Engine::GameObject*>& newTargets);
    void SetObstacles(const std::vector<Engine::GameObject*>& newObstacles);

   private:
    void CheckTargets();
    bool CheckObstacles();

   private:
    Engine::TransformComponent* transform = nullptr;

    Engine::Vector2Df direction = {0.f, 0.f};

    std::vector<Engine::GameObject*> targets;
    std::vector<Engine::GameObject*> obstacles;

    std::string textureKey;
    float damage = 10.f;
    float speed = 700.f;
    float radius = 8.f;
    float lifeTime = 1.5f;
    float textureWidth = 64.f;
    float textureHeight = 34.f;
};
}  // namespace Roguelike
