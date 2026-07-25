// @file UIProgressBar.h

#pragma once

#include "UIElement.h"

namespace Engine
{
class UIProgressBar : public UIElement
{
   public:
    UIProgressBar();

    void SetPosition(sf::Vector2f position);
    void SetSize(sf::Vector2f size);
    void SetValue(float current, float max);

    void SetBackColor(sf::Color color);
    void SetFillColor(sf::Color color);
    void SetOutlineColor(sf::Color color);

    void Draw(sf::RenderWindow& window) override;

   private:
    sf::RectangleShape background;
    sf::RectangleShape fill;

    sf::Color backColor = sf::Color(30, 25, 25, 220);
    sf::Color fillColor = sf::Color(180, 55, 55, 255);
    sf::Color outlineColor = sf::Color(230, 200, 140, 255);

    float value = 1.0f;
};
}  // namespace Engine