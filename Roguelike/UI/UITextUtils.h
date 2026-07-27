#pragma once

#include <SFML/Graphics.hpp>

#include <string>

namespace Roguelike::UITextUtils
{
std::string FitTextToWidth(const sf::Font& font,
                           const std::string& text,
                           unsigned int characterSize,
                           float maxWidth);

std::string WrapText(const sf::Font& font,
                     const std::string& text,
                     unsigned int characterSize,
                     float maxWidth,
                     int maxLines);
}  // namespace Roguelike::UITextUtils
