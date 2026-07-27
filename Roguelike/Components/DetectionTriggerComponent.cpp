#include "pch.h"
#include "DetectionTriggerComponent.h"

#include "RenderSystem.h"
#include "PhysicsSystem.h"

namespace Roguelike
{
DetectionTriggerComponent::DetectionTriggerComponent(Engine::GameObject* gameObject)
    : ColliderComponent(gameObject)
{
    transform = gameObject->GetComponent<Engine::TransformComponent>();

    SetTrigger(true);

    Engine::PhysicsSystem::Instance()->Subscribe(this);
}

DetectionTriggerComponent::~DetectionTriggerComponent()
{
    Engine::PhysicsSystem::Instance()->Unsubscribe(this);
}

void DetectionTriggerComponent::Update(float deltaTime)
{
    if (transform == nullptr)
    {
        return;
    }

    Engine::Vector2Df position = transform->GetWorldPosition();

    bounds = sf::FloatRect(position.x - radius, position.y - radius, radius * 2.f, radius * 2.f);
}

void DetectionTriggerComponent::Render()
{
    if (!showDebug || transform == nullptr)
    {
        return;
    }

    Engine::Vector2Df position = transform->GetWorldPosition();

    // DEBUG: enemy detection radius visualization.
    sf::CircleShape circle(radius);
    circle.setOrigin(radius, radius);
    circle.setPosition(position.x, position.y);
    circle.setFillColor(sf::Color::Transparent);
    circle.setOutlineColor(sf::Color(0, 255, 0, 120));
    circle.setOutlineThickness(2.f);

    Engine::RenderSystem::Instance()->Render(circle);
}

void DetectionTriggerComponent::SetRadius(float newRadius) { radius = newRadius; }

void DetectionTriggerComponent::SetShowDebug(bool value) { showDebug = value; }
}  // namespace Roguelike