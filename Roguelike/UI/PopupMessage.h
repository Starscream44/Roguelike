#pragma once

#include "UIElement.h"

#include <SFML/Graphics.hpp>

#include <string>

namespace Roguelike
{
class PopupMessage : public Engine::UIElement
{
   public:
    explicit PopupMessage(const sf::Font& font);

    void ShowMessage(const std::string& message, float duration = 2.0f);

    void Update(float deltaTime) override;
    void Draw(sf::RenderWindow& window) override;

   private:
    const sf::Font& font;

    // Background is a fallback rectangle; PopupMessage.cpp draws the texture when available.
    sf::RectangleShape background;
    sf::Text text;

    // Lifetime controls auto-hide; fade alpha is handled by UIAnimation.
    float timer = 0.0f;
    float lifeTime = 2.0f;
};
}  // namespace Roguelike
