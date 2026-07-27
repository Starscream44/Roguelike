#pragma once

#include "Component.h"
#include "GameConfig.h"
#include "InputComponent.h"
#include "RigidbodyComponent.h"
#include "Vector.h"

namespace Roguelike
{
class PlayerMovementComponent : public Engine::Component
{
   public:
    PlayerMovementComponent(Engine::GameObject* gameObject);

    void Update(float deltaTime) override;
    void Render() override;

    void SetSpeed(float newSpeed);
    float GetSpeed() const;

   private:
    Engine::InputComponent* input = nullptr;
    Engine::RigidbodyComponent* rigidbody = nullptr;

    float speed = GameConfig::PlayerMoveSpeed;
};
}  // namespace Roguelike
