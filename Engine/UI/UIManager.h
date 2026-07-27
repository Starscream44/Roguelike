#pragma once

#include "UIElement.h"

#include <memory>
#include <utility>
#include <vector>

namespace Engine
{
class UIManager
{
   public:
    template <typename T, typename... Args>
    T& CreateElement(Args&&... args)
    {
        auto element = std::make_unique<T>(std::forward<Args>(args)...);
        T& reference = *element;

        elements.push_back(std::move(element));

        return reference;
    }

    void HandleEvent(const sf::Event& event);
    void Update(float deltaTime);
    void Draw(sf::RenderWindow& window);
    void Clear();

   private:
    std::vector<std::unique_ptr<UIElement>> elements;
};
}  // namespace Engine