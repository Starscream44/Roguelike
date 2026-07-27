#pragma once

#include "UIItemView.h"

#include <SFML/Graphics.hpp>

#include <string>

namespace Roguelike::UITextureUtils
{
// Shared UI texture helper keeps panels focused on layout and state.
bool HasTexture(const std::string& textureKey);

bool DrawTexture(sf::RenderWindow& window,
                 const std::string& textureKey,
                 sf::FloatRect bounds,
                 sf::Uint8 alpha,
                 sf::Color tint = sf::Color::White,
                 float fillRatio = 1.0f,
                 sf::BlendMode blendMode = sf::BlendAlpha);

bool DrawNineSliceTexture(sf::RenderWindow& window,
                          const std::string& textureKey,
                          sf::FloatRect bounds,
                          float leftMargin,
                          float topMargin,
                          float rightMargin,
                          float bottomMargin,
                          sf::Uint8 alpha,
                          sf::Color tint = sf::Color::White,
                          sf::BlendMode blendMode = sf::BlendAlpha);

bool DrawItemTexture(sf::RenderWindow& window,
                     const UIItemView& item,
                     sf::FloatRect bounds,
                     sf::Uint8 alpha);
}  // namespace Roguelike::UITextureUtils
