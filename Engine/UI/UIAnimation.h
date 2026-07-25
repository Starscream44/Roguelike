// @file UIAnimation.h

#pragma once

namespace Engine
{
class UIAnimation
{
   public:
    // Visibility commands animate toward target alpha instead of snapping instantly.
    void Show();
    void Hide();
    void Update(float deltaTime);

    // fadeSpeed is alpha units per second. Higher values make UI appear/disappear faster.
    void SetFadeSpeed(float speed);
    void SetAlpha(float value);

    bool IsVisible() const;
    bool IsFullyHidden() const;

    float GetAlpha() const;

   private:
    bool isVisible = true;

    // alpha is the current opacity; targetAlpha is where Update() moves it.
    float alpha = 255.0f;
    float targetAlpha = 255.0f;
    float fadeSpeed = 700.0f;
};
}  // namespace Engine
