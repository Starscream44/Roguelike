#include "pch.h"
#include "AttackComponent.h"

#include "GameObject.h"
#include "StatsComponent.h"
#include "Logger.h"

#include <string>

namespace Engine
{
AttackComponent::AttackComponent(GameObject* gameObject) : Component(gameObject)
{
    LOG_INFO(gameObject->GetName() + " AttackComponent created.");
}

void AttackComponent::Update(float deltaTime) {}

void AttackComponent::Render() {}

void AttackComponent::SetAttackPower(float newAttackPower)
{
    if (newAttackPower < 0.f)
    {
        LOG_WARN(gameObject->GetName() + " attack power cannot be negative.");
        return;
    }

    attackPower = newAttackPower;

    LOG_INFO(gameObject->GetName() + " attack power set to " + std::to_string(attackPower));
}

float AttackComponent::GetAttackPower() const { return attackPower; }

bool AttackComponent::Attack(GameObject* target)
{
    if (target == nullptr)
    {
        LOG_ERROR(gameObject->GetName() + " attack failed. Target is null.");
        return false;
    }

    StatsComponent* targetStats = target->GetComponent<StatsComponent>();

    if (targetStats == nullptr)
    {
        LOG_WARN(gameObject->GetName() + " attack skipped. Target has no StatsComponent.");
        return false;
    }

    if (targetStats->IsDead())
    {
        return false;
    }

    float finalDamage = targetStats->TakeDamage(attackPower);

    LOG_INFO(gameObject->GetName() + " attacked " + target->GetName() +
             ". Damage: " + std::to_string(finalDamage) +
             ", target HP: " + std::to_string(targetStats->GetHealth()));

    return true;
}
}  // namespace Engine