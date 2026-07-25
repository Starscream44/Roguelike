// @file PhysicsSystem.h

#pragma once

#include <set>
#include <utility>
#include <vector>

namespace Engine
{
class ColliderComponent;

class PhysicsSystem
{
   public:
    static PhysicsSystem* Instance();

    void Update();

    float GetFixedDeltaTime() const;

    void Subscribe(ColliderComponent* collider);
    void Unsubscribe(ColliderComponent* collider);

   private:
    PhysicsSystem() {}
    ~PhysicsSystem() {}

    PhysicsSystem(PhysicsSystem const&) = delete;
    PhysicsSystem& operator=(PhysicsSystem const&) = delete;

    using TriggerPair = std::pair<ColliderComponent*, ColliderComponent*>;

    // Trigger contacts belong to a collider pair, not to one collider.
    TriggerPair MakeTriggerPair(ColliderComponent* first, ColliderComponent* second) const;
    bool IsTriggerPairActive(ColliderComponent* first, ColliderComponent* second) const;
    void RemoveTriggerPairsWith(ColliderComponent* collider);

    std::vector<ColliderComponent*> colliders;
    std::set<TriggerPair> activeTriggerPairs;

    float fixedDeltaTime = 0.02f;
};
}  // namespace Engine
