#pragma once

#include "Component.h"

namespace Engine
{
class GameObject;

class AttackComponent : public Component
{
   public:
    AttackComponent(GameObject* gameObject);

    void Update(float deltaTime) override;
    void Render() override;

    void SetAttackPower(float newAttackPower);
    float GetAttackPower() const;

    bool Attack(GameObject* target);

   private:
    float attackPower = 10.f;
};
}  // namespace Engine