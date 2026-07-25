// @file CameraComponent.h

#pragma once

#include "Component.h"
#include "TransformComponent.h"

#include <SFML/Graphics.hpp>

namespace Engine
{
class CameraComponent : public Component
{
   public:
    CameraComponent(GameObject* gameObject);

    void Update(float deltaTime) override;
    void Render() override;

    void SetWindow(sf::RenderWindow* newWindow);
    void SetBaseResolution(float newBaseWidth, float newBaseHeight);

   private:
    TransformComponent* transform = nullptr;
    sf::RenderWindow* window = nullptr;

    float baseWidth = 1280.f;
    float baseHeight = 720.f;
};
}  // namespace Engine