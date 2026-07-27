#include "pch.h"
#include "UIManager.h"

namespace Engine
{
void UIManager::HandleEvent(const sf::Event& event)
{
    // Events are delivered in creation order. Modal behavior is handled by game UI state,
    // not by reordering elements here.
    for (auto& element : elements)
    {
        if (element->IsVisible())
        {
            element->HandleEvent(event);
        }
    }
}

void UIManager::Update(float deltaTime)
{
    // Hidden elements still update so fade-out animations can finish cleanly.
    for (auto& element : elements)
    {
        element->Update(deltaTime);
    }
}

void UIManager::Draw(sf::RenderWindow& window)
{
    // Draw order matches creation order; later elements appear on top.
    for (auto& element : elements)
    {
        if (element->IsVisible())
        {
            element->Draw(window);
        }
    }
}

void UIManager::Clear() { elements.clear(); }
}  // namespace Engine
