// @file Trigger.cpp

#include "pch.h"
#include "Trigger.h"
#include "ColliderComponent.h"

namespace Engine
{
Trigger::Trigger(ColliderComponent* newFirst, ColliderComponent* newSecond)
    : first(newFirst), second(newSecond)
{
}

ColliderComponent* Trigger::GetFirst() const { return first; }

ColliderComponent* Trigger::GetSecond() const { return second; }

bool Trigger::HasGameObject(GameObject* gameObject) const
{
    return first->GetGameObject() == gameObject || second->GetGameObject() == gameObject;
}
}  // namespace Engine