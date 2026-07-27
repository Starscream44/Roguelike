#include "pch.h"
#include "ItemDescriptionPanel.h"

#include "UITextureUtils.h"
#include "UITextUtils.h"

namespace Roguelike
{
namespace
{
// Item description panel layout. Change these values to move/resize the bottom info block.
const sf::FloatRect PanelBounds = {580.0f, 720.0f, 760.0f, 230.0f};
const sf::FloatRect IconBounds = {640.0f, 770.0f, 120.0f, 120.0f};

// Text positions are absolute screen coordinates.
const sf::Vector2f NameTextPosition = {770.0f, 770.0f};
const sf::Vector2f CountTextPosition = {770.0f, 800.0f};
const sf::Vector2f DescriptionTextPosition = {770.0f, 840.0f};

// Text sizes and wrapping limits.
const unsigned int NameTextSize = 24;
const unsigned int CountTextSize = 14;
const unsigned int DescriptionTextSize = 14;
const float NameMaxWidth = 460.0f;
const float DescriptionMaxWidth = 500.0f;
const int DescriptionMaxLines = 2;
}  // namespace

ItemDescriptionPanel::ItemDescriptionPanel(const sf::Font& uiFont)
    : FramedPanel(uiFont), font(uiFont)
{
    SetupFrame({PanelBounds.left, PanelBounds.top}, {PanelBounds.width, PanelBounds.height}, "");

    icon.setPosition({IconBounds.left, IconBounds.top});
    icon.setSize({IconBounds.width, IconBounds.height});

    nameText.setFont(font);
    nameText.setCharacterSize(NameTextSize);
    nameText.setPosition(NameTextPosition);

    descriptionText.setFont(font);
    descriptionText.setCharacterSize(DescriptionTextSize);
    descriptionText.setPosition(DescriptionTextPosition);

    countText.setFont(font);
    countText.setCharacterSize(CountTextSize);
    countText.setPosition(CountTextPosition);
}

void ItemDescriptionPanel::ShowItem(const UIItemView& item)
{
    selectedItem = item;

    // Long names/descriptions are clipped or wrapped here so they never leave the panel.
    nameText.setString(
        UITextUtils::FitTextToWidth(font, selectedItem.stack.GetName(), NameTextSize, NameMaxWidth));
    descriptionText.setString(
        UITextUtils::WrapText(font, selectedItem.stack.GetDescription(), DescriptionTextSize,
                              DescriptionMaxWidth, DescriptionMaxLines));
    countText.setString("Count: " + std::to_string(selectedItem.stack.count));

    icon.setFillColor(selectedItem.iconColor);

    Show();
}

void ItemDescriptionPanel::Draw(sf::RenderWindow& window)
{
    sf::Uint8 alpha = GetAlphaByte();

    // Prefer the authored description texture; DrawFrame keeps the panel usable if art is missing.
    if (!UITextureUtils::DrawTexture(window, "ui_inventory_menu_description", PanelBounds, alpha))
    {
        DrawFrame(window);
    }

    bool drewItemTexture = UITextureUtils::DrawItemTexture(window, selectedItem, IconBounds, alpha);

    sf::Color iconColor = selectedItem.iconColor;
    iconColor.a = alpha;
    icon.setFillColor(iconColor);

    nameText.setFillColor(sf::Color(232, 214, 140, alpha));
    descriptionText.setFillColor(sf::Color(206, 198, 134, alpha));
    countText.setFillColor(sf::Color(196, 186, 118, alpha));

    if (!drewItemTexture)
    {
        window.draw(icon);
    }

    window.draw(nameText);
    window.draw(countText);
    window.draw(descriptionText);
}
}  // namespace Roguelike
