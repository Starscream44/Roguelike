#include "pch.h"
#include "PickupComponent.h"

#include "GameObject.h"
#include "PhysicsSystem.h"
#include "Trigger.h"

namespace Roguelike
{
PickupComponent::PickupComponent(Engine::GameObject* gameObject) : ColliderComponent(gameObject)
{
    SetTrigger(true);
    SubscribeTriggerEnter([this](Engine::Trigger trigger) { HandleTrigger(trigger); });

    Engine::PhysicsSystem::Instance()->Subscribe(this);
}

PickupComponent::~PickupComponent() { Engine::PhysicsSystem::Instance()->Unsubscribe(this); }

bool PickupComponent::IsCollected() const { return isCollected; }

void PickupComponent::HandleTrigger(Engine::Trigger trigger)
{
    if (isCollected)
    {
        return;
    }

    Engine::GameObject* firstObject = trigger.GetFirst()->GetGameObject();
    Engine::GameObject* secondObject = trigger.GetSecond()->GetGameObject();
    Engine::GameObject* collectorObject = firstObject == gameObject ? secondObject : firstObject;

    isCollected = Collect(collectorObject);
}
}  // namespace Roguelike
