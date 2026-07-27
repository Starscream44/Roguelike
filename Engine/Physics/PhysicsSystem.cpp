#include "pch.h"
#include "PhysicsSystem.h"

#include "ColliderComponent.h"
#include "RigidbodyComponent.h"
#include "TransformComponent.h"

namespace Engine
{
PhysicsSystem* PhysicsSystem::Instance()
{
    static PhysicsSystem physicsSystem;
    return &physicsSystem;
}

float PhysicsSystem::GetFixedDeltaTime() const { return fixedDeltaTime; }

void PhysicsSystem::Update()
{
    for (int i = 0; i < static_cast<int>(colliders.size()); i++)
    {
        auto firstBody = colliders[i]->GetGameObject()->GetComponent<RigidbodyComponent>();

        if (firstBody == nullptr || firstBody->GetKinematic())
        {
            continue;
        }

        for (int j = 0; j < static_cast<int>(colliders.size()); j++)
        {
            if (i == j)
            {
                continue;
            }

            if (colliders[i]->GetGameObject() == colliders[j]->GetGameObject())
            {
                continue;
            }

            sf::FloatRect intersection;

            if (!colliders[i]->bounds.intersects(colliders[j]->bounds, intersection))
            {
                continue;
            }

            if (colliders[i]->isTrigger || colliders[j]->isTrigger)
            {
                // Fire Enter only once while the same two colliders keep touching.
                if (!IsTriggerPairActive(colliders[i], colliders[j]))
                {
                    Trigger trigger(colliders[i], colliders[j]);

                    colliders[i]->OnTriggerEnter(trigger);
                    colliders[j]->OnTriggerEnter(trigger);

                    activeTriggerPairs.insert(MakeTriggerPair(colliders[i], colliders[j]));
                }

                continue;
            }

            auto secondBody = colliders[j]->GetGameObject()->GetComponent<RigidbodyComponent>();

            if (secondBody != nullptr && !secondBody->GetKinematic())
            {
                continue;
            }

            auto transform = colliders[i]->GetGameObject()->GetComponent<TransformComponent>();

            if (transform == nullptr)
            {
                continue;
            }

            Vector2Df velocity = firstBody->GetLinearVelocity();

            float firstCenterX = colliders[i]->bounds.left + colliders[i]->bounds.width * 0.5f;

            float firstCenterY = colliders[i]->bounds.top + colliders[i]->bounds.height * 0.5f;

            float secondCenterX = colliders[j]->bounds.left + colliders[j]->bounds.width * 0.5f;

            float secondCenterY = colliders[j]->bounds.top + colliders[j]->bounds.height * 0.5f;

            float offsetX = firstCenterX - secondCenterX;
            float offsetY = firstCenterY - secondCenterY;

            if (intersection.width < intersection.height)
            {
                transform->MoveBy(offsetX < 0.f ? -intersection.width : intersection.width, 0.f);

                velocity.x = 0.f;
            }
            else
            {
                transform->MoveBy(0.f, offsetY < 0.f ? -intersection.height : intersection.height);

                velocity.y = 0.f;
            }

            firstBody->SetLinearVelocity(velocity);

            Collision collision(colliders[i], colliders[j], intersection);

            colliders[i]->OnCollision(collision);
            colliders[j]->OnCollision(collision);
        }
    }

    for (auto triggeredPair = activeTriggerPairs.cbegin(), nextTriggeredPair = triggeredPair;
         triggeredPair != activeTriggerPairs.cend(); triggeredPair = nextTriggeredPair)
    {
        ++nextTriggeredPair;

        // Fire Exit when a previously active trigger pair separates.
        if (!triggeredPair->first->bounds.intersects(triggeredPair->second->bounds))
        {
            Trigger trigger(triggeredPair->first, triggeredPair->second);

            triggeredPair->first->OnTriggerExit(trigger);
            triggeredPair->second->OnTriggerExit(trigger);

            activeTriggerPairs.erase(triggeredPair);
        }
    }
}

void PhysicsSystem::Subscribe(ColliderComponent* collider)
{
    if (collider == nullptr)
    {
        return;
    }

    if (std::find(colliders.begin(), colliders.end(), collider) != colliders.end())
    {
        return;
    }

    colliders.push_back(collider);
}

void PhysicsSystem::Unsubscribe(ColliderComponent* collider)
{
    if (collider == nullptr)
    {
        return;
    }

    colliders.erase(std::remove_if(colliders.begin(), colliders.end(),
                                   [collider](ColliderComponent* obj) { return obj == collider; }),
                    colliders.end());

    RemoveTriggerPairsWith(collider);
}

PhysicsSystem::TriggerPair PhysicsSystem::MakeTriggerPair(ColliderComponent* first,
                                                          ColliderComponent* second) const
{
    // Store trigger pairs in one stable order so A-B and B-A are the same contact.
    return first < second ? TriggerPair(first, second) : TriggerPair(second, first);
}

bool PhysicsSystem::IsTriggerPairActive(ColliderComponent* first, ColliderComponent* second) const
{
    return activeTriggerPairs.find(MakeTriggerPair(first, second)) != activeTriggerPairs.end();
}

void PhysicsSystem::RemoveTriggerPairsWith(ColliderComponent* collider)
{
    // Removed colliders must not leave stale pointers in active trigger contacts.
    for (auto triggeredPair = activeTriggerPairs.cbegin(), nextTriggeredPair = triggeredPair;
         triggeredPair != activeTriggerPairs.cend(); triggeredPair = nextTriggeredPair)
    {
        ++nextTriggeredPair;

        if (triggeredPair->first == collider || triggeredPair->second == collider)
        {
            activeTriggerPairs.erase(triggeredPair);
        }
    }
}
}  // namespace Engine
