// @file BoxColliderComponent.cpp

#include "pch.h"
#include "BoxColliderComponent.h"

#include "RenderSystem.h"

namespace Engine
{
BoxColliderComponent::BoxColliderComponent(GameObject* gameObject) : ColliderComponent(gameObject)
{
    transform = gameObject->GetComponent<TransformComponent>();
    PhysicsSystem::Instance()->Subscribe(this);
}

BoxColliderComponent::~BoxColliderComponent()
{
    PhysicsSystem::Instance()->Unsubscribe(this);
}

void BoxColliderComponent::Update(float deltaTime)
{
    if (transform == nullptr)
    {
        return;
    }

    const Vector2Df position = transform->GetWorldPosition();

    // Box collider is centered on Transform, matching SpriteRenderer tile origin.
    bounds = {position.x - size.x * 0.5f, position.y - size.y * 0.5f, size.x, size.y};
}

void BoxColliderComponent::Render()
{
    if (!showDebug)
    {
        return;
    }

    sf::RectangleShape rectangle(sf::Vector2f(bounds.width, bounds.height));

    rectangle.setPosition(bounds.left, bounds.top);
    rectangle.setFillColor(sf::Color::Transparent);
    rectangle.setOutlineColor(sf::Color::Yellow);
    rectangle.setOutlineThickness(2.0f);

    RenderSystem::Instance()->Render(rectangle);
}

void BoxColliderComponent::SetSize(float width, float height)
{
    size = {width, height};
}

void BoxColliderComponent::SetShowDebug(bool value)
{
    showDebug = value;
}
}  // namespace Engine
