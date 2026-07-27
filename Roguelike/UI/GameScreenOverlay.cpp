#include "pch.h"
#include "GameScreenOverlay.h"

#include "GameConfig.h"
#include "UITextureUtils.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace Roguelike
{
namespace
{
// Centers SFML text visually around a screen position after its string or size changes.
void CenterText(sf::Text& text, sf::Vector2f position)
{
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
    text.setPosition(position);
}
}  // namespace

GameScreenOverlay::GameScreenOverlay(const sf::Font& uiFont, const sf::Font& uiTitleFont)
    : font(uiFont), titleFont(uiTitleFont)
{
    background.setPosition({0.0f, 0.0f});
    background.setSize({GameConfig::WindowWidth, GameConfig::WindowHeight});

    // Fallback rectangular panel for non-textured overlay screens.
    windowPanel.setSize({820.0f, 420.0f});
    windowPanel.setPosition({GameConfig::WindowCenterX - 410.0f,
                             GameConfig::WindowCenterY - 210.0f});
    windowPanel.setFillColor(sf::Color(18, 16, 18, 235));
    windowPanel.setOutlineColor(sf::Color(180, 150, 100, 255));
    windowPanel.setOutlineThickness(4.0f);

    // Pause popup placement. X is centered on the screen; Y can be nudged here.
    pauseTextPanel.setSize({PausePanelWidth, PausePanelHeight});
    pauseTextPanel.setOrigin({PausePanelWidth * 0.5f, PausePanelHeight * 0.5f});
    pauseTextPanel.setPosition({GameConfig::WindowCenterX, GameConfig::WindowCenterY - 10.0f});
    pauseTextPanel.setFillColor(sf::Color(6, 6, 8, 165));
    pauseTextPanel.setOutlineColor(sf::Color(244, 226, 146, 70));
    pauseTextPanel.setOutlineThickness(2.0f);

    // Default title/subtitle sizes for menu, game over, and level complete screens.
    // Pause overrides these sizes in Draw() because it uses a smaller textured popup.
    titleText.setFont(titleFont);
    titleText.setCharacterSize(64);
    titleText.setFillColor(sf::Color::White);

    subtitleText.setFont(font);
    subtitleText.setCharacterSize(28);
    subtitleText.setFillColor(sf::Color::White);

    GetAnimation().SetFadeSpeed(MaxAlpha / MainMenuFadeSeconds);
    GetAnimation().SetAlpha(0.0f);
    Hide();
}

void GameScreenOverlay::ShowMainMenu()
{
    style = OverlayStyle::MainMenu;
    SetText("ADVENTURES OF MIRINDELLA", "Press Space to begin");
    isMainMenuBackdropOpaque = true;
    GetAnimation().SetAlpha(0.0f);
    Show();
}

void GameScreenOverlay::ShowPause()
{
    style = OverlayStyle::Pause;
    SetText("PAUSE", "Press Esc to continue");
    isMainMenuBackdropOpaque = false;
    Show();
}

void GameScreenOverlay::ShowGameOver()
{
    style = OverlayStyle::GameOver;
    SetText("YOU DIED", "Press Space to rise again");
    isMainMenuBackdropOpaque = false;
    deathLightTime = 0.0f;
    GetAnimation().SetAlpha(0.0f);
    Show();
}

void GameScreenOverlay::ShowLevelCleared(int nextLevel)
{
    style = OverlayStyle::LevelCleared;
    SetText("PATH CLEARED", "Press Space to enter level " + std::to_string(nextLevel));
    isMainMenuBackdropOpaque = false;
    GetAnimation().SetAlpha(0.0f);
    Show();
}

void GameScreenOverlay::HideOverlay()
{
    isMainMenuBackdropOpaque = false;
    Hide();
}

void GameScreenOverlay::Update(float deltaTime)
{
    Engine::UIElement::Update(deltaTime);

    if (IsVisible())
    {
        subtitleBlinkTime += deltaTime;
    }

    if (style == OverlayStyle::GameOver && IsVisible())
    {
        deathLightTime += deltaTime;
    }

    if (style == OverlayStyle::MainMenu && IsVisible())
    {
        fireLightTime += deltaTime;
        fireSpriteTime += deltaTime;
    }
}

void GameScreenOverlay::SetText(const std::string& title, const std::string& subtitle)
{
    titleText.setString(title);
    subtitleText.setString(subtitle);

    // Default text placement for full-screen overlays.
    // Pause text is repositioned in Draw() to fit the smaller popup.
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.left + titleBounds.width * 0.5f,
                        titleBounds.top + titleBounds.height * 0.5f);
    titleText.setPosition({GameConfig::WindowCenterX, GameConfig::WindowCenterY - 72.0f});

    sf::FloatRect subtitleBounds = subtitleText.getLocalBounds();
    subtitleText.setOrigin(subtitleBounds.left + subtitleBounds.width * 0.5f,
                           subtitleBounds.top + subtitleBounds.height * 0.5f);
    subtitleText.setPosition({GameConfig::WindowCenterX, GameConfig::WindowCenterY + 34.0f});
}

const char* GameScreenOverlay::GetBackgroundTextureKey() const
{
    switch (style)
    {
        case OverlayStyle::MainMenu:
            return "ui_start_game_background";
        case OverlayStyle::GameOver:
            return "ui_dead_background";
        case OverlayStyle::LevelCleared:
            return "ui_next_level_background";
        case OverlayStyle::Pause:
        default:
            return nullptr;
    }
}

float GameScreenOverlay::GetEnterOffset() const
{
    float progress = static_cast<float>(GetAlpha()) / MaxAlpha;
    progress = std::clamp(progress, 0.0f, 1.0f);

    float easedProgress = 1.0f - (1.0f - progress) * (1.0f - progress);
    return EnterOffsetY * (1.0f - easedProgress);
}

sf::Uint8 GameScreenOverlay::GetDeathLightAlpha(sf::Uint8 backgroundAlpha) const
{
    if (style != OverlayStyle::GameOver)
    {
        return 0;
    }

    float progress = std::clamp(deathLightTime / DeathLightFadeSeconds, 0.0f, 1.0f);
    float fade = (1.0f - progress) * (1.0f - progress);

    std::uint32_t jitterStep = static_cast<std::uint32_t>(deathLightTime * 12.0f);
    float jitter = (Noise01(jitterStep + 71) - 0.5f) * DeathLightJitterAlpha * fade;
    float tremble = std::sin(deathLightTime * 17.0f) * 9.0f * fade;

    float alpha = static_cast<float>(backgroundAlpha) * fade + jitter + tremble;
    return static_cast<sf::Uint8>(std::clamp(alpha, 0.0f, MaxAlpha));
}

sf::Uint8 GameScreenOverlay::GetFireLightAlpha(sf::Uint8 backgroundAlpha) const
{
    if (style != OverlayStyle::MainMenu)
    {
        return 0;
    }

    std::uint32_t jitterStep = static_cast<std::uint32_t>(fireLightTime * 18.0f);
    float jitter = (Noise01(jitterStep) - 0.5f) * 0.32f;

    float sharpPulse = std::max(0.0f, std::sin(fireLightTime * 29.0f +
                                               Noise01(jitterStep + 17) * 6.28318f));
    sharpPulse = sharpPulse * sharpPulse * sharpPulse * 0.22f;

    float lowWave = 0.52f + 0.15f * std::sin(fireLightTime * 5.7f) +
                    0.08f * std::sin(fireLightTime * 11.3f);
    float flicker = lowWave + jitter + sharpPulse;
    flicker = std::clamp(flicker, 0.0f, 1.0f);

    float alpha = FireLightMinAlpha + (FireLightMaxAlpha - FireLightMinAlpha) * flicker;
    alpha *= static_cast<float>(backgroundAlpha) / MaxAlpha;

    return static_cast<sf::Uint8>(std::clamp(alpha, 0.0f, MaxAlpha));
}

sf::Uint8 GameScreenOverlay::GetSubtitleAlpha(sf::Uint8 alpha) const
{
    float wave = (std::sin(subtitleBlinkTime * SubtitleBlinkSpeed) + 1.0f) * 0.5f;
    float factor = SubtitleMinAlphaFactor + (1.0f - SubtitleMinAlphaFactor) * wave;
    float result = static_cast<float>(alpha) * factor;

    return static_cast<sf::Uint8>(std::clamp(result, 0.0f, MaxAlpha));
}

sf::FloatRect GameScreenOverlay::GetStartBackgroundBounds(const sf::RenderWindow& window,
                                                          float strength) const
{
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    sf::Vector2u windowSize = window.getSize();

    float normalizedX =
        std::clamp((static_cast<float>(mousePosition.x) / static_cast<float>(windowSize.x)) -
                       0.5f,
                   -0.5f, 0.5f);
    float normalizedY =
        std::clamp((static_cast<float>(mousePosition.y) / static_cast<float>(windowSize.y)) -
                       0.5f,
                   -0.5f, 0.5f);

    float width = GameConfig::WindowWidth + StartBackgroundOverscan * 2.0f;
    float height = GameConfig::WindowHeight + StartBackgroundOverscan * 2.0f;
    float x = -StartBackgroundOverscan - normalizedX * strength;
    float y = -StartBackgroundOverscan - normalizedY * strength;

    return {x, y, width, height};
}

sf::FloatRect GameScreenOverlay::GetStartFireBounds(const sf::RenderWindow& window) const
{
    sf::FloatRect backgroundBounds = GetStartBackgroundBounds(window, StartBackgroundParallax);
    float parallaxX = backgroundBounds.left + StartBackgroundOverscan;
    float parallaxY = backgroundBounds.top + StartBackgroundOverscan;

    return {GameConfig::StartMenuFireLeft + parallaxX, GameConfig::StartMenuFireTop + parallaxY,
            GameConfig::StartMenuFireWidth, GameConfig::StartMenuFireHeight};
}

std::string GameScreenOverlay::GetStartFireTextureKey() const
{
    int frameIndex = static_cast<int>(fireSpriteTime / GameConfig::StartMenuFireFrameSeconds) %
                     GameConfig::StartMenuFireFrameCount;

    return "ui_start_fire_" + std::to_string(frameIndex + 1);
}

void GameScreenOverlay::DrawMainMenuEffects(sf::RenderWindow& window,
                                            sf::Uint8 backgroundAlpha) const
{
    UITextureUtils::DrawTexture(window, GetStartFireTextureKey(), GetStartFireBounds(window),
                                backgroundAlpha);

    sf::FloatRect fireLightBounds = GetStartBackgroundBounds(window, FireLightParallax);
    UITextureUtils::DrawTexture(window, "ui_start_game_fire_light", fireLightBounds,
                                GetFireLightAlpha(backgroundAlpha), sf::Color::White, 1.0f,
                                sf::BlendAdd);
}

void GameScreenOverlay::DrawGameOverEffects(sf::RenderWindow& window,
                                            sf::Uint8 backgroundAlpha) const
{
    UITextureUtils::DrawTexture(window, "ui_dead_light_background",
                                {0.0f, 0.0f, GameConfig::WindowWidth,
                                 GameConfig::WindowHeight},
                                GetDeathLightAlpha(backgroundAlpha), sf::Color::White, 1.0f,
                                sf::BlendAlpha);
}

void GameScreenOverlay::DrawPausePanel(sf::RenderWindow& window, float enterOffset,
                                       sf::Uint8 alpha) const
{
    sf::RectangleShape animatedPausePanel = pauseTextPanel;
    animatedPausePanel.move({0.0f, enterOffset * 0.55f});
    sf::FloatRect pausePanelBounds = animatedPausePanel.getGlobalBounds();

    // The pause popup uses 9-slice drawing so the decorative corners do not stretch.
    if (UITextureUtils::DrawNineSliceTexture(window, "ui_popup_message", pausePanelBounds,
                                             PausePanelTextureMarginX,
                                             PausePanelTextureMarginTop,
                                             PausePanelTextureMarginX,
                                             PausePanelTextureMarginBottom,
                                             static_cast<sf::Uint8>(alpha * 0.88f)))
    {
        return;
    }

    animatedPausePanel.setFillColor(sf::Color(6, 6, 8, static_cast<sf::Uint8>(alpha * 0.66f)));
    animatedPausePanel.setOutlineColor(
        sf::Color(244, 226, 146, static_cast<sf::Uint8>(alpha * 0.28f)));
    window.draw(animatedPausePanel);
}

float GameScreenOverlay::Noise01(std::uint32_t seed)
{
    seed ^= seed >> 16;
    seed *= 0x7feb352dU;
    seed ^= seed >> 15;
    seed *= 0x846ca68bU;
    seed ^= seed >> 16;

    return static_cast<float>(seed & 0x00ffffffU) / static_cast<float>(0x00ffffffU);
}

void GameScreenOverlay::Draw(sf::RenderWindow& window)
{
    sf::Uint8 alpha = GetAlphaByte();
    sf::Uint8 backgroundAlpha = isMainMenuBackdropOpaque ? 255 : alpha;
    float enterOffset = GetEnterOffset();

    background.setFillColor(sf::Color(0, 0, 0, backgroundAlpha));

    sf::Color panelColor = sf::Color(18, 16, 18, alpha);
    sf::Color outlineColor = sf::Color(180, 150, 100, alpha);

    sf::RectangleShape animatedPanel = windowPanel;
    animatedPanel.move({0.0f, enterOffset});
    animatedPanel.setFillColor(panelColor);
    animatedPanel.setOutlineColor(outlineColor);

    sf::Color titleColor = style == OverlayStyle::MainMenu
                               ? sf::Color(255, 255, 255, alpha)
                               : sf::Color(244, 226, 146, alpha);
    sf::Color subtitleColor = style == OverlayStyle::MainMenu
                                  ? sf::Color(255, 255, 255, GetSubtitleAlpha(alpha))
                                  : sf::Color(220, 210, 190, GetSubtitleAlpha(alpha));

    sf::Text animatedTitle = titleText;
    sf::Text animatedSubtitle = subtitleText;

    if (style == OverlayStyle::Pause)
    {
        // Pause-specific text tuning:
        // - character sizes are in GameScreenOverlay.h;
        // - Y offsets below move text inside the popup.
        animatedTitle.setCharacterSize(PauseTitleCharacterSize);
        animatedSubtitle.setCharacterSize(PauseSubtitleCharacterSize);
        CenterText(animatedTitle, {GameConfig::WindowCenterX, GameConfig::WindowCenterY - 52.0f});
        CenterText(animatedSubtitle, {GameConfig::WindowCenterX, GameConfig::WindowCenterY + 20.0f});
    }

    animatedTitle.move({0.0f, enterOffset});
    animatedSubtitle.move({0.0f, enterOffset * 0.65f});
    animatedTitle.setFillColor(titleColor);
    animatedSubtitle.setFillColor(subtitleColor);

    const char* backgroundTextureKey = GetBackgroundTextureKey();
    sf::FloatRect backgroundBounds =
        style == OverlayStyle::MainMenu
            ? GetStartBackgroundBounds(window, StartBackgroundParallax)
            : sf::FloatRect{0.0f, 0.0f, GameConfig::WindowWidth, GameConfig::WindowHeight};
    bool drewBackgroundTexture =
        backgroundTextureKey != nullptr &&
        UITextureUtils::DrawTexture(window, backgroundTextureKey, backgroundBounds,
                                    backgroundAlpha);

    if (style == OverlayStyle::MainMenu && drewBackgroundTexture)
    {
        DrawMainMenuEffects(window, backgroundAlpha);
    }

    if (style == OverlayStyle::GameOver && drewBackgroundTexture)
    {
        DrawGameOverEffects(window, backgroundAlpha);
    }

    if (!drewBackgroundTexture && style != OverlayStyle::Pause)
    {
        window.draw(background);
        window.draw(animatedPanel);
    }

    if (style == OverlayStyle::Pause)
    {
        DrawPausePanel(window, enterOffset, alpha);
    }

    window.draw(animatedTitle);
    window.draw(animatedSubtitle);
}
}  // namespace Roguelike
