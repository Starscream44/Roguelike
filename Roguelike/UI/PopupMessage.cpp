#include "pch.h"
#include "PopupMessage.h"

#include "UITextureUtils.h"

#include <algorithm>

namespace Roguelike
{
namespace
{
// Popup layout. Base positions are screen-space coordinates; text position is separate
// because the decorative texture has asymmetric empty space.
const sf::Vector2f PopupBasePosition = {20.0f, 260.0f};
const sf::Vector2f PopupTextBasePosition = {64.0f, 320.0f};
const sf::Vector2f PopupFallbackSize = {320.0f, 140.0f};

// Popup motion. The message slides in while fading, then floats upward near the end.
const float PopupEnterOffsetY = 18.0f;
const float PopupExitFloatY = -12.0f;
const float PopupExitStartProgress = 0.72f;
}  // namespace

PopupMessage::PopupMessage(const sf::Font& uiFont) : font(uiFont)
{
    // Fallback rectangle is used only when the popup texture is not loaded.
    background.setSize(PopupFallbackSize);
    background.setPosition(PopupBasePosition);
    background.setFillColor(sf::Color(25, 22, 20, 210));
    background.setOutlineColor(sf::Color(120, 90, 65, 255));
    background.setOutlineThickness(2.0f);

    text.setFont(font);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);
    text.setPosition(PopupTextBasePosition);

    GetAnimation().SetAlpha(0.0f);
    Hide();
}

void PopupMessage::ShowMessage(const std::string& message, float duration)
{
    text.setString(message);

    lifeTime = duration;
    timer = 0.0f;

    Show();
}

void PopupMessage::Update(float deltaTime)
{
    Engine::UIElement::Update(deltaTime);

    if (!IsVisible())
    {
        return;
    }

    timer += deltaTime;

    if (timer >= lifeTime)
    {
        Hide();
    }
}

void PopupMessage::Draw(sf::RenderWindow& window)
{
    sf::Uint8 alpha = GetAlphaByte();
    float fadeProgress = static_cast<float>(alpha) / 255.0f;
    float lifeProgress = lifeTime > 0.0f ? std::clamp(timer / lifeTime, 0.0f, 1.0f) : 1.0f;
    float enterOffset = PopupEnterOffsetY * (1.0f - fadeProgress);
    float exitOffset = PopupExitFloatY * std::max(0.0f, lifeProgress - PopupExitStartProgress) /
                       (1.0f - PopupExitStartProgress);
    float yOffset = enterOffset + exitOffset;

    // Alpha is inherited from UIAnimation; color RGB stays constant while opacity changes.
    sf::Color backgroundColor = sf::Color(25, 22, 20, 210);
    backgroundColor.a = static_cast<sf::Uint8>((210.0f / 255.0f) * alpha);

    background.setPosition({PopupBasePosition.x, PopupBasePosition.y + yOffset});
    background.setFillColor(backgroundColor);
    background.setOutlineColor(sf::Color(120, 90, 65, alpha));
    text.setPosition({PopupTextBasePosition.x, PopupTextBasePosition.y + yOffset});
    text.setFillColor(sf::Color(255, 255, 255, alpha));

    if (!UITextureUtils::DrawTexture(window, "ui_popup_message",
                                     {background.getPosition().x, background.getPosition().y,
                                      background.getSize().x, background.getSize().y},
                                     alpha))
    {
        window.draw(background);
    }

    window.draw(text);
}
}  // namespace Roguelike
