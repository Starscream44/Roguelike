#include "pch.h"
#include "StatsComponent.h"

#include <algorithm>

namespace Engine
{
StatsComponent::StatsComponent(GameObject* gameObject) : Component(gameObject) {}

void StatsComponent::Update(float deltaTime) {}

void StatsComponent::Render() {}

void StatsComponent::SetStats(float newHealth, float newArmor)
{
    health = newHealth;
    armor = newArmor;
    NotifyStatsChanged();
}

void StatsComponent::SetAttackPower(float newAttackPower) { attackPower = newAttackPower; }

void StatsComponent::AddStatsChangedListener(StatsChangedCallback callback)
{
    if (callback)
    {
        statsChangedListeners.push_back(std::move(callback));
    }
}

float StatsComponent::TakeDamage(float damage)
{
    if (armor >= 1.0f)
    {
        armor -= 1.0f;
        NotifyStatsChanged();
        return 0.0f;
    }

    float finalDamage = std::max(0.0f, damage);
    health -= finalDamage;

    if (health < 0.0f)
    {
        health = 0.0f;
    }

    NotifyStatsChanged();
    return finalDamage;
}

float StatsComponent::GetHealth() const { return health; }

float StatsComponent::GetArmor() const { return armor; }

float StatsComponent::GetAttackPower() const { return attackPower; }

bool StatsComponent::IsDead() const { return health <= 0.0f; }

void StatsComponent::NotifyStatsChanged()
{
    for (const StatsChangedCallback& listener : statsChangedListeners)
    {
        if (listener)
        {
            listener(health, armor);
        }
    }
}
}  // namespace Engine
