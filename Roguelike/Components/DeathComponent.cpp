#include "pch.h"
#include "DeathComponent.h"

#include "GameObject.h"
#include "GameWorld.h"
#include "StatsComponent.h"
#include "Logger.h"

namespace Engine
{
DeathComponent::DeathComponent(GameObject* gameObject) : Component(gameObject)
{
    stats = gameObject->GetComponent<StatsComponent>();

    if (stats == nullptr)
    {
        LOG_WARN(gameObject->GetName() + " DeathComponent created without StatsComponent.");
    }
}

void DeathComponent::Update(float deltaTime)
{
    if (isMarkedToDestroy || stats == nullptr)
    {
        return;
    }

    if (stats->IsDead())
    {
        isMarkedToDestroy = true;

        LOG_INFO("GameObject marked to destroy: " + gameObject->GetName());

        GameWorld::Instance()->DestroyGameObject(gameObject);
    }
}

void DeathComponent::Render() {}
}  // namespace Engine