#pragma once

#include "UIAnimation.h"

#include <SFML/Graphics.hpp>

namespace Engine
{
class UIElement
{
   public:
    virtual ~UIElement() = default;

    virtual void HandleEvent(const sf::Event& event);
    virtual void Update(float deltaTime);
    virtual void Draw(sf::RenderWindow& window) = 0;

    void Show();
    void Hide();
    void SetVisible(bool value);

    bool IsVisible() const;

    float GetAlpha() const;
    sf::Uint8 GetAlphaByte() const;

    UIAnimation& GetAnimation();

   protected:
    sf::Color ApplyAlpha(sf::Color color) const;

   private:
    UIAnimation animation;
};
}  // namespace Engine