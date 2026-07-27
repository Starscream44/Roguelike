#pragma once

#include "FramedPanel.h"
#include "UIItemView.h"

#include <SFML/Graphics.hpp>

namespace Roguelike
{
class ItemDescriptionPanel : public FramedPanel
{
   public:
    explicit ItemDescriptionPanel(const sf::Font& font);

    void ShowItem(const UIItemView& item);
    void Draw(sf::RenderWindow& window) override;

   private:
    const sf::Font& font;

    sf::RectangleShape icon;

    sf::Text nameText;
    sf::Text descriptionText;
    sf::Text countText;

    UIItemView selectedItem;
};
}  // namespace Roguelike
