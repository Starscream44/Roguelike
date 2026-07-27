#pragma once

#include "UIElement.h"
#include "UIProgressBar.h"

#include <SFML/Graphics.hpp>

namespace Roguelike
{
class HUD : public Engine::UIElement
{
   public:
    HUD(const sf::Font& font, const sf::Font& titleFont);

    void SetStats(float health, float maxHealth, float armor, float maxArmor);
    void SetObjective(int level, int aliveEnemies, int totalEnemies);
    void Draw(sf::RenderWindow& window) override;

   private:
    const sf::Font& font;
    const sf::Font& titleFont;

    // Text objects are positioned in HUD.cpp because the parchment and bar layouts are art-driven.
    sf::Text healthText;
    sf::Text armorText;
    sf::Text levelText;
    sf::Text objectiveText;

    Engine::UIProgressBar healthBar;
    Engine::UIProgressBar armorBar;

    // Cached gameplay values. Draw() formats these into the visible HUD strings.
    float currentHealth = 100.0f;
    float maximumHealth = 100.0f;
    float currentArmor = 0.0f;
    float maximumArmor = 100.0f;
    int currentLevel = 1;
    int currentAliveEnemies = 0;
    int currentTotalEnemies = 0;
};
}  // namespace Roguelike
