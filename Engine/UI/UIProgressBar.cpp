#include "pch.h"
#include "UIProgressBar.h"

#include <algorithm>

namespace Engine
{
UIProgressBar::UIProgressBar() { background.setOutlineThickness(2.0f); }

void UIProgressBar::SetPosition(sf::Vector2f position)
{
    background.setPosition(position);
    fill.setPosition(position);
}

void UIProgressBar::SetSize(sf::Vector2f size)
{
    background.setSize(size);
    fill.setSize(size);
}

void UIProgressBar::SetValue(float current, float max)
{
    // Store progress normalized to 0..1; rendering scales the fill rectangle later.
    if (max <= 0.0f)
    {
        value = 0.0f;
        return;
    }

    value = std::clamp(current / max, 0.0f, 1.0f);
}

void UIProgressBar::SetBackColor(sf::Color color) { backColor = color; }

void UIProgressBar::SetFillColor(sf::Color color) { fillColor = color; }

void UIProgressBar::SetOutlineColor(sf::Color color) { outlineColor = color; }

void UIProgressBar::Draw(sf::RenderWindow& window)
{
    // ApplyAlpha comes from UIElement, so bars fade together with their parent HUD.
    background.setFillColor(ApplyAlpha(backColor));
    background.setOutlineColor(ApplyAlpha(outlineColor));

    sf::Vector2f size = background.getSize();

    // Fill shrinks horizontally while the background frame keeps its full size.
    fill.setSize({size.x * value, size.y});
    fill.setFillColor(ApplyAlpha(fillColor));

    window.draw(background);
    window.draw(fill);
}
}  // namespace Engine
