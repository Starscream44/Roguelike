#pragma once

#include "Component.h"

#include <functional>
#include <utility>
#include <vector>

namespace Engine
{
class StatsComponent : public Component
{
   public:
    using StatsChangedCallback = std::function<void(float health, float armor)>;

    explicit StatsComponent(GameObject* gameObject);

    void Update(float deltaTime) override;
    void Render() override;

    void SetStats(float newHealth, float newArmor);
    void SetAttackPower(float newAttackPower);

    // UI and gameplay helpers subscribe here instead of polling health every frame.
    void AddStatsChangedListener(StatsChangedCallback callback);

    float TakeDamage(float damage);

    float GetHealth() const;
    float GetArmor() const;
    float GetAttackPower() const;

    bool IsDead() const;

   private:
    void NotifyStatsChanged();

   private:
    std::vector<StatsChangedCallback> statsChangedListeners;

    float health = 100.0f;
    float armor = 0.0f;
    float attackPower = 10.0f;
};
}  // namespace Engine
