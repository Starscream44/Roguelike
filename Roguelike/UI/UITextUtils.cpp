#include "pch.h"
#include "UITextUtils.h"

#include <sstream>

namespace Roguelike::UITextUtils
{
// Measures rendered text with the exact SFML font and character size used by the UI.
static float GetTextWidth(const sf::Font& font, const std::string& text, unsigned int characterSize)
{
    sf::Text measuredText;
    measuredText.setFont(font);
    measuredText.setCharacterSize(characterSize);
    measuredText.setString(text);

    return measuredText.getLocalBounds().width;
}

std::string FitTextToWidth(const sf::Font& font,
                           const std::string& text,
                           unsigned int characterSize,
                           float maxWidth)
{
    if (GetTextWidth(font, text, characterSize) <= maxWidth)
    {
        return text;
    }

    std::string fitted;

    // Keep adding characters until the ellipsis would overflow maxWidth.
    for (char character : text)
    {
        std::string candidate = fitted + character + "...";

        if (GetTextWidth(font, candidate, characterSize) > maxWidth)
        {
            break;
        }

        fitted += character;
    }

    return fitted + "...";
}

std::string WrapText(const sf::Font& font,
                     const std::string& text,
                     unsigned int characterSize,
                     float maxWidth,
                     int maxLines)
{
    std::istringstream words(text);
    std::string word;
    std::string line;
    std::string wrapped;
    int lineCount = 1;

    // Word wrapping keeps descriptions readable and avoids text leaving inventory panels.
    while (words >> word)
    {
        std::string candidate = line.empty() ? word : line + " " + word;

        if (GetTextWidth(font, candidate, characterSize) <= maxWidth)
        {
            line = candidate;
            continue;
        }

        if (!wrapped.empty())
        {
            wrapped += "\n";
        }

        wrapped += FitTextToWidth(font, line, characterSize, maxWidth);
        line = word;
        lineCount++;

        if (lineCount > maxLines)
        {
            return FitTextToWidth(font, wrapped, characterSize, maxWidth);
        }
    }

    if (!line.empty())
    {
        if (!wrapped.empty())
        {
            wrapped += "\n";
        }

        wrapped += FitTextToWidth(font, line, characterSize, maxWidth);
    }

    return wrapped;
}
}  // namespace Roguelike::UITextUtils
