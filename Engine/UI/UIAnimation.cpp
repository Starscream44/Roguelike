// @file UIAnimation.cpp

#include "pch.h"
#include "UIAnimation.h"

#include <algorithm>

namespace Engine
{
void UIAnimation::Show()
{
    isVisible = true;
    targetAlpha = 255.0f;
}

void UIAnimation::Hide()
{
    isVisible = false;
    targetAlpha = 0.0f;
}

void UIAnimation::Update(float deltaTime)
{
    // Simple linear fade shared by all UIElement-based widgets.
    if (alpha < targetAlpha)
    {
        alpha += fadeSpeed * deltaTime;
        alpha = std::min(alpha, targetAlpha);
    }
    else if (alpha > targetAlpha)
    {
        alpha -= fadeSpeed * deltaTime;
        alpha = std::max(alpha, targetAlpha);
    }
}

void UIAnimation::SetFadeSpeed(float speed) { fadeSpeed = speed; }

void UIAnimation::SetAlpha(float value)
{
    alpha = std::clamp(value, 0.0f, 255.0f);
    targetAlpha = alpha;
    isVisible = alpha > 0.0f;
}

bool UIAnimation::IsVisible() const { return isVisible || alpha > 0.0f; }

bool UIAnimation::IsFullyHidden() const { return alpha <= 0.0f; }

float UIAnimation::GetAlpha() const { return alpha; }
}  // namespace Engine
