#pragma once

#include "UIElement.h"

#include <SFML/Graphics.hpp>

#include <cstdint>
#include <string>

namespace Roguelike
{
class GameScreenOverlay : public Engine::UIElement
{
   public:
    GameScreenOverlay(const sf::Font& font, const sf::Font& titleFont);

    void ShowMainMenu();
    void ShowPause();
    void ShowGameOver();
    void ShowLevelCleared(int nextLevel);
    void HideOverlay();

    void Update(float deltaTime) override;
    void Draw(sf::RenderWindow& window) override;

   private:
    enum class OverlayStyle
    {
        MainMenu,
        Pause,
        GameOver,
        LevelCleared
    };

    void SetText(const std::string& title, const std::string& subtitle);
    const char* GetBackgroundTextureKey() const;
    float GetEnterOffset() const;
    sf::Uint8 GetDeathLightAlpha(sf::Uint8 backgroundAlpha) const;
    sf::Uint8 GetFireLightAlpha(sf::Uint8 backgroundAlpha) const;
    sf::Uint8 GetSubtitleAlpha(sf::Uint8 alpha) const;
    sf::FloatRect GetStartBackgroundBounds(const sf::RenderWindow& window, float strength) const;
    sf::FloatRect GetStartFireBounds(const sf::RenderWindow& window) const;
    std::string GetStartFireTextureKey() const;
    void DrawMainMenuEffects(sf::RenderWindow& window, sf::Uint8 backgroundAlpha) const;
    void DrawGameOverEffects(sf::RenderWindow& window, sf::Uint8 backgroundAlpha) const;
    void DrawPausePanel(sf::RenderWindow& window, float enterOffset, sf::Uint8 alpha) const;
    static float Noise01(std::uint32_t seed);

   private:
    // Shared overlay animation tuning.
    static constexpr float MainMenuFadeSeconds = 1.0f;
    static constexpr float MaxAlpha = 255.0f;
    static constexpr float EnterOffsetY = 34.0f;

    // Main menu background and fire parallax tuning.
    static constexpr float FireLightMinAlpha = 16.0f;
    static constexpr float FireLightMaxAlpha = 92.0f;
    static constexpr float StartBackgroundOverscan = 28.0f;
    static constexpr float StartBackgroundParallax = 20.0f;
    static constexpr float FireLightParallax = 28.0f;

    // Subtitle blink animation tuning.
    static constexpr float SubtitleBlinkSpeed = 2.2f;
    static constexpr float SubtitleMinAlphaFactor = 0.34f;

    // Game over light fade and jitter tuning.
    static constexpr float DeathLightFadeSeconds = 3.8f;
    static constexpr float DeathLightJitterAlpha = 34.0f;

    // Pause popup layout. Width/height control the panel size.
    // Texture margins are 9-slice borders: corners keep their original shape,
    // only the middle stretches.
    static constexpr float PausePanelWidth = 600.0f;
    static constexpr float PausePanelHeight = 210.0f;
    static constexpr float PausePanelTextureMarginX = 92.0f;
    static constexpr float PausePanelTextureMarginTop = 78.0f;
    static constexpr float PausePanelTextureMarginBottom = 82.0f;

    // Pause text sizes. Text positions are adjusted in Draw() for the pause style.
    static constexpr unsigned int PauseTitleCharacterSize = 52;
    static constexpr unsigned int PauseSubtitleCharacterSize = 26;

    const sf::Font& font;
    const sf::Font& titleFont;

    sf::RectangleShape background;
    sf::RectangleShape windowPanel;
    sf::RectangleShape pauseTextPanel;

    sf::Text titleText;
    sf::Text subtitleText;

    OverlayStyle style = OverlayStyle::MainMenu;
    bool isMainMenuBackdropOpaque = false;
    float deathLightTime = 0.0f;
    float fireLightTime = 0.0f;
    float fireSpriteTime = 0.0f;
    float subtitleBlinkTime = 0.0f;
};
}  // namespace Roguelike
