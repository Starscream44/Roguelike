#include "pch.h"
#include "FramedPanel.h"

namespace Roguelike
{
namespace
{
// Generic fallback frame styling for inventory-like panels when custom textures are unavailable.
const sf::Color FrameBackgroundColor = sf::Color(38, 54, 38, 236);
const sf::Color FrameOutlineColor = sf::Color(145, 142, 86, 255);
const sf::Color FrameTitleColor = sf::Color(226, 210, 132);
const float FrameOutlineThickness = 3.0f;
const unsigned int FrameTitleSize = 26;
const sf::Vector2f FrameTitleOffset = {24.0f, 18.0f};
}  // namespace

FramedPanel::FramedPanel(const sf::Font& uiFont) : font(uiFont)
{
    background.setFillColor(FrameBackgroundColor);
    background.setOutlineColor(FrameOutlineColor);
    background.setOutlineThickness(FrameOutlineThickness);

    titleText.setFont(font);
    titleText.setCharacterSize(FrameTitleSize);
    titleText.setFillColor(FrameTitleColor);

    GetAnimation().SetAlpha(0.0f);
    Hide();
}

void FramedPanel::SetupFrame(sf::Vector2f position, sf::Vector2f size, const std::string& title)
{
    background.setPosition(position);
    background.setSize(size);

    titleText.setString(title);
    titleText.setPosition(position.x + FrameTitleOffset.x, position.y + FrameTitleOffset.y);
}

void FramedPanel::DrawFrame(sf::RenderWindow& window)
{
    sf::Uint8 alpha = GetAlphaByte();

    // The frame is redrawn every frame with current fade alpha from UIAnimation.
    background.setFillColor(sf::Color(FrameBackgroundColor.r, FrameBackgroundColor.g,
                                      FrameBackgroundColor.b, alpha));
    background.setOutlineColor(sf::Color(FrameOutlineColor.r, FrameOutlineColor.g,
                                         FrameOutlineColor.b, alpha));
    titleText.setFillColor(sf::Color(FrameTitleColor.r, FrameTitleColor.g, FrameTitleColor.b,
                                     alpha));

    window.draw(background);
    window.draw(titleText);
}

sf::Vector2f FramedPanel::GetPosition() const { return background.getPosition(); }

sf::Vector2f FramedPanel::GetSize() const { return background.getSize(); }
}  // namespace Roguelike
