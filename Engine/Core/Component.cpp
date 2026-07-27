#include "pch.h"
#include "Component.h"

namespace Engine
{
Component::Component(GameObject* newGameObject) { gameObject = newGameObject; }

Component::~Component() {}

void Component::HandleEvent(const sf::Event& event) {}

GameObject* Component::GetGameObject() const { return gameObject; }
}  // namespace Engine
