#include "pch.h"
#include "CameraComponent.h"

namespace Engine
{
CameraComponent::CameraComponent(GameObject* gameObject) : Component(gameObject)
{
    transform = gameObject->GetComponent<TransformComponent>();
}

void CameraComponent::Update(float deltaTime)
{
    if (window == nullptr || transform == nullptr)
    {
        return;
    }

    sf::View view;

    view.setSize(baseWidth, baseHeight);

    Vector2Df cameraPosition = transform->GetWorldPosition();

    view.setCenter(cameraPosition.x, cameraPosition.y);

    window->setView(view);
}

void CameraComponent::Render() {}

void CameraComponent::SetWindow(sf::RenderWindow* newWindow) { window = newWindow; }

void CameraComponent::SetBaseResolution(float newBaseWidth, float newBaseHeight)
{
    baseWidth = newBaseWidth;
    baseHeight = newBaseHeight;
}
}  // namespace Engine