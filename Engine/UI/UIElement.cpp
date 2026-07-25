// @file UIElement.cpp

#include "pch.h"
#include "UIElement.h"

namespace Engine
{
void UIElement::HandleEvent(const sf::Event& event) {}

void UIElement::Update(float deltaTime) { animation.Update(deltaTime); }

void UIElement::Show() { animation.Show(); }

void UIElement::Hide() { animation.Hide(); }

void UIElement::SetVisible(bool value)
{
    if (value)
    {
        Show();
    }
    else
    {
        Hide();
    }
}

bool UIElement::IsVisible() const { return animation.IsVisible(); }

float UIElement::GetAlpha() const { return animation.GetAlpha(); }

sf::Uint8 UIElement::GetAlphaByte() const { return static_cast<sf::Uint8>(GetAlpha()); }

UIAnimation& UIElement::GetAnimation() { return animation; }

sf::Color UIElement::ApplyAlpha(sf::Color color) const
{
    color.a = GetAlphaByte();
    return color;
}
}  // namespace Engine