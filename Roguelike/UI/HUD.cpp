#include "pch.h"
#include "HUD.h"

#include "GameConfig.h"
#include "UITextureUtils.h"

#include <algorithm>

namespace Roguelike
{
namespace
{
bool DrawTexturedBar(sf::RenderWindow& window,
                     sf::FloatRect bounds,
                     float value,
                     float maxValue,
                     sf::Uint8 alpha,
                     sf::Color fillTint)
{
    const float ratio = maxValue > 0.0f ? value / maxValue : 0.0f;
    const bool drewFrame =
        UITextureUtils::DrawTexture(window, "ui_hud_health_bar_frame", bounds, alpha);
    const bool drewFill =
        UITextureUtils::DrawTexture(window, "ui_hud_health_bar_fill", bounds, alpha, fillTint,
                                    ratio);
    return drewFrame && drewFill;
}

bool DrawArmorIcon(sf::RenderWindow& window, sf::FloatRect bounds, sf::Uint8 alpha)
{
    return UITextureUtils::DrawTexture(window, "ui_player_armor", bounds, alpha);
}

// HUD layout. These are absolute screen coordinates in the 1920x1080 UI space.
// Move the health bar, armor icons, and quest parchment by changing these values.
const sf::FloatRect HealthBarBounds = {20.0f, 18.0f, 440.0f, 108.0f};
const sf::Vector2f ArmorStartPosition = {112.0f, 128.0f};
const sf::Vector2f ArmorIconSize = {64.0f, 64.0f};
const float ArmorIconGap = 72.0f;

// Quest parchment layout. Width/height resize the paper texture;
// margin controls the distance from the top-right corner.
const float QuestPanelWidth = 190.0f;
const float QuestPanelHeight = 174.0f;
const float QuestPanelMargin = 24.0f;
const sf::FloatRect QuestPanelBounds = {GameConfig::WindowWidth - QuestPanelWidth -
                                            QuestPanelMargin,
                                        QuestPanelMargin, QuestPanelWidth, QuestPanelHeight};
// Text Y positions inside the quest parchment.
const float QuestLevelY = QuestPanelBounds.top + 46.0f;
const float QuestEnemiesY = QuestPanelBounds.top + 100.0f;

void CenterText(sf::Text& text, float centerX, float y)
{
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
    text.setPosition(centerX, y);
}
}  // namespace

HUD::HUD(const sf::Font& uiFont, const sf::Font& uiTitleFont)
    : font(uiFont), titleFont(uiTitleFont)
{
    healthText.setFont(font);
    healthText.setCharacterSize(24);
    healthText.setPosition(24.0f, 54.0f);

    armorText.setFont(font);
    armorText.setCharacterSize(24);
    armorText.setPosition(24.0f, 136.0f);

    // Quest title and objective text sizes.
    levelText.setFont(titleFont);
    levelText.setCharacterSize(18);

    objectiveText.setFont(font);
    objectiveText.setCharacterSize(15);

    healthBar.SetPosition({120.0f, 22.0f});
    healthBar.SetSize({180.0f, 18.0f});
    healthBar.SetFillColor(sf::Color(185, 55, 55, 255));

    armorBar.SetPosition({120.0f, 56.0f});
    armorBar.SetSize({180.0f, 18.0f});
    armorBar.SetFillColor(sf::Color(95, 125, 170, 255));
}

void HUD::SetStats(float health, float maxHealth, float armor, float maxArmor)
{
    currentHealth = health;
    maximumHealth = maxHealth;
    currentArmor = armor;
    maximumArmor = maxArmor;

    healthBar.SetValue(health, maxHealth);
    armorBar.SetValue(armor, maxArmor);
}

void HUD::SetObjective(int level, int aliveEnemies, int totalEnemies)
{
    // GameUIComponent passes alive/total enemies. HUD converts it to killed/total in Draw().
    currentLevel = level;
    currentAliveEnemies = aliveEnemies;
    currentTotalEnemies = totalEnemies;
}

void HUD::Draw(sf::RenderWindow& window)
{
    healthText.setString("");
    armorText.setString("");
    levelText.setString("LEVEL " + std::to_string(currentLevel));
    // Quest progress is displayed as killed enemies, e.g. "Kill enemies: 0/5".
    int killedEnemies = std::max(0, currentTotalEnemies - currentAliveEnemies);
    objectiveText.setString("Kill enemies: " + std::to_string(killedEnemies) + "/" +
                            std::to_string(currentTotalEnemies));

    float questCenterX = QuestPanelBounds.left + QuestPanelBounds.width * 0.5f;
    CenterText(levelText, questCenterX, QuestLevelY);
    CenterText(objectiveText, questCenterX, QuestEnemiesY);

    healthText.setFillColor(ApplyAlpha(sf::Color::White));
    armorText.setFillColor(ApplyAlpha(sf::Color::White));
    levelText.setFillColor(ApplyAlpha(sf::Color(79, 52, 31, 255)));
    objectiveText.setFillColor(ApplyAlpha(sf::Color(65, 48, 36, 255)));

    sf::Uint8 alpha = GetAlphaByte();
    bool drewHealthBar =
        DrawTexturedBar(window, HealthBarBounds, currentHealth, maximumHealth, alpha,
                        sf::Color::White);
    bool drewArmor = false;
    int armorIconCount =
        static_cast<int>(std::min(std::max(0.0f, currentArmor),
                                  std::max(0.0f, maximumArmor)));

    if (UITextureUtils::HasTexture("ui_player_armor"))
    {
        for (int i = 0; i < armorIconCount; ++i)
        {
            DrawArmorIcon(window,
                          {ArmorStartPosition.x + static_cast<float>(i) * ArmorIconGap,
                           ArmorStartPosition.y, ArmorIconSize.x, ArmorIconSize.y},
                          alpha);
        }

        drewArmor = true;
    }

    if (!drewHealthBar || !drewArmor)
    {
        healthBar.GetAnimation().SetAlpha(GetAlpha());
        armorBar.GetAnimation().SetAlpha(GetAlpha());

        if (!drewHealthBar)
        {
            healthBar.Draw(window);
        }

        if (!drewArmor)
        {
            armorBar.Draw(window);
        }
    }

    UITextureUtils::DrawTexture(window, "ui_quest_list", QuestPanelBounds, alpha);
    window.draw(healthText);
    window.draw(armorText);
    window.draw(levelText);
    window.draw(objectiveText);
}
}  // namespace Roguelike
