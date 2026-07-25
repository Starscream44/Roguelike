// @file Component.h

#pragma once

#include <SFML/Window/Event.hpp>

namespace Engine
{
class GameObject;

class Component
{
   public:
    Component(GameObject* gameObject);
    virtual ~Component();

    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;
    virtual void HandleEvent(const sf::Event& event);

    GameObject* GetGameObject() const;

   protected:
    GameObject* gameObject = nullptr;
};
}  // namespace Engine
