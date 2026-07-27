#pragma once

#include "UIElement.h"

#include <SFML/Graphics.hpp>

#include <string>

namespace Roguelike
{
class FramedPanel : public Engine::UIElement
{
   protected:
    explicit FramedPanel(const sf::Font& font);

    void SetupFrame(sf::Vector2f position, sf::Vector2f size, const std::string& title);
    void DrawFrame(sf::RenderWindow& window);

    sf::Vector2f GetPosition() const;
    sf::Vector2f GetSize() const;

   private:
    const sf::Font& font;

    sf::RectangleShape background;
    sf::Text titleText;
};
}  // namespace Roguelike
