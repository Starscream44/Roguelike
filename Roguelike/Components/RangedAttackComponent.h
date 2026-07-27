#pragma once

#include "Component.h"

#include <string>
#include <vector>

namespace Engine
{
class GameObject;
class TransformComponent;
}  // namespace Engine

namespace Roguelike
{
class RangedAttackComponent : public Engine::Component
{
   public:
    RangedAttackComponent(Engine::GameObject* gameObject);

    void Update(float deltaTime) override;
    void Render() override;

    void AddTarget(Engine::GameObject* target);
    void SetTargets(const std::vector<Engine::GameObject*>& newTargets);
    void SetObstacles(const std::vector<Engine::GameObject*>& newObstacles);
    void SetProjectileTextureKey(const std::string& newTextureKey);

   private:
    void Shoot();

   private:
    Engine::TransformComponent* transform = nullptr;

    std::vector<Engine::GameObject*> targets;
    std::vector<Engine::GameObject*> obstacles;

    std::string projectileTextureKey;
    bool wasLeftMousePressed = false;
};
}  // namespace Roguelike
