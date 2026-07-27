#include "pch.h"
#include "UITextureUtils.h"

#include "ResourceSystem.h"

#include <algorithm>

namespace Roguelike::UITextureUtils
{
namespace
{
sf::Sprite& GetReusableSprite()
{
    static sf::Sprite sprite;
    return sprite;
}

// Draws one texture fragment into one destination fragment. Nine-slice and progress bars
// both rely on this small primitive so texture math stays in one place.
void DrawTexturePatch(sf::RenderWindow& window,
                      const sf::Texture& texture,
                      sf::IntRect source,
                      sf::FloatRect destination,
                      sf::Color color,
                      sf::BlendMode blendMode)
{
    if (source.width <= 0 || source.height <= 0 || destination.width <= 0.0f ||
        destination.height <= 0.0f)
    {
        return;
    }

    sf::Sprite& sprite = GetReusableSprite();
    sprite.setTexture(texture);
    sprite.setTextureRect(source);
    sprite.setPosition({destination.left, destination.top});
    sprite.setScale(destination.width / static_cast<float>(source.width),
                    destination.height / static_cast<float>(source.height));
    sprite.setColor(color);
    window.draw(sprite, blendMode);
}
}  // namespace

bool HasTexture(const std::string& textureKey)
{
    return Engine::ResourceSystem::Instance()->HasTexture(textureKey);
}

bool DrawTexture(sf::RenderWindow& window,
                 const std::string& textureKey,
                 sf::FloatRect bounds,
                 sf::Uint8 alpha,
                 sf::Color tint,
                 float fillRatio,
                 sf::BlendMode blendMode)
{
    Engine::ResourceSystem* resources = Engine::ResourceSystem::Instance();

    if (!resources->HasTexture(textureKey))
    {
        return false;
    }

    const sf::Texture* texture = resources->GetTextureShared(textureKey);
    sf::Vector2u textureSize = texture->getSize();

    if (textureSize.x == 0 || textureSize.y == 0)
    {
        return false;
    }

    // fillRatio crops the source from the right. HUD health fill uses this for progress.
    fillRatio = std::clamp(fillRatio, 0.0f, 1.0f);
    if (fillRatio <= 0.0f)
    {
        return true;
    }

    const int sourceWidth =
        std::max(1, static_cast<int>(static_cast<float>(textureSize.x) * fillRatio));

    sf::Sprite& sprite = GetReusableSprite();
    sprite.setTexture(*texture);
    sprite.setTextureRect({0, 0, sourceWidth, static_cast<int>(textureSize.y)});
    sprite.setPosition({bounds.left, bounds.top});
    sprite.setScale(bounds.width / static_cast<float>(textureSize.x),
                    bounds.height / static_cast<float>(textureSize.y));
    sprite.setColor(sf::Color(tint.r, tint.g, tint.b,
                              static_cast<sf::Uint8>((static_cast<float>(tint.a) / 255.0f) *
                                                     static_cast<float>(alpha))));

    window.draw(sprite, blendMode);
    return true;
}

bool DrawNineSliceTexture(sf::RenderWindow& window,
                          const std::string& textureKey,
                          sf::FloatRect bounds,
                          float leftMargin,
                          float topMargin,
                          float rightMargin,
                          float bottomMargin,
                          sf::Uint8 alpha,
                          sf::Color tint,
                          sf::BlendMode blendMode)
{
    Engine::ResourceSystem* resources = Engine::ResourceSystem::Instance();

    if (!resources->HasTexture(textureKey))
    {
        return false;
    }

    const sf::Texture* texture = resources->GetTextureShared(textureKey);
    sf::Vector2u textureSize = texture->getSize();

    if (textureSize.x == 0 || textureSize.y == 0)
    {
        return false;
    }

    // Margins describe fixed decorative borders in source pixels. The center can stretch,
    // corners stay crisp, and margins are clamped so tiny panels cannot invert the slices.
    const float maxHorizontalMargin = std::max(1.0f, static_cast<float>(textureSize.x) * 0.5f - 1.0f);
    const float maxVerticalMargin = std::max(1.0f, static_cast<float>(textureSize.y) * 0.5f - 1.0f);
    leftMargin = std::clamp(leftMargin, 1.0f, maxHorizontalMargin);
    rightMargin = std::clamp(rightMargin, 1.0f, maxHorizontalMargin);
    topMargin = std::clamp(topMargin, 1.0f, maxVerticalMargin);
    bottomMargin = std::clamp(bottomMargin, 1.0f, maxVerticalMargin);

    const float maxDestinationHorizontalMargin = std::max(1.0f, bounds.width * 0.5f - 1.0f);
    const float maxDestinationVerticalMargin = std::max(1.0f, bounds.height * 0.5f - 1.0f);
    const float destinationLeft = std::min(leftMargin, maxDestinationHorizontalMargin);
    const float destinationRight = std::min(rightMargin, maxDestinationHorizontalMargin);
    const float destinationTop = std::min(topMargin, maxDestinationVerticalMargin);
    const float destinationBottom = std::min(bottomMargin, maxDestinationVerticalMargin);

    const int sourceLeft = static_cast<int>(leftMargin);
    const int sourceRight = static_cast<int>(rightMargin);
    const int sourceTop = static_cast<int>(topMargin);
    const int sourceBottom = static_cast<int>(bottomMargin);
    const int sourceCenterWidth =
        static_cast<int>(textureSize.x) - sourceLeft - sourceRight;
    const int sourceCenterHeight =
        static_cast<int>(textureSize.y) - sourceTop - sourceBottom;

    const float x[4] = {bounds.left,
                        bounds.left + destinationLeft,
                        bounds.left + bounds.width - destinationRight,
                        bounds.left + bounds.width};
    const float y[4] = {bounds.top,
                        bounds.top + destinationTop,
                        bounds.top + bounds.height - destinationBottom,
                        bounds.top + bounds.height};

    const int sx[4] = {0,
                       sourceLeft,
                       static_cast<int>(textureSize.x) - sourceRight,
                       static_cast<int>(textureSize.x)};
    const int sy[4] = {0,
                       sourceTop,
                       static_cast<int>(textureSize.y) - sourceBottom,
                       static_cast<int>(textureSize.y)};

    sf::Color color(tint.r, tint.g, tint.b,
                    static_cast<sf::Uint8>((static_cast<float>(tint.a) / 255.0f) *
                                           static_cast<float>(alpha)));

    for (int row = 0; row < 3; ++row)
    {
        for (int column = 0; column < 3; ++column)
        {
            DrawTexturePatch(window, *texture,
                             {sx[column], sy[row], sx[column + 1] - sx[column],
                              sy[row + 1] - sy[row]},
                             {x[column], y[row], x[column + 1] - x[column],
                              y[row + 1] - y[row]},
                             color, blendMode);
        }
    }

    return sourceCenterWidth > 0 && sourceCenterHeight > 0;
}

bool DrawItemTexture(sf::RenderWindow& window,
                     const UIItemView& item,
                     sf::FloatRect bounds,
                     sf::Uint8 alpha)
{
    const std::string textureKey = GetItemTextureKey(item.stack);
    return !textureKey.empty() && DrawTexture(window, textureKey, bounds, alpha);
}
}  // namespace Roguelike::UITextureUtils
