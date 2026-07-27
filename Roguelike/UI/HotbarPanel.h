#pragma once

#include "UIElement.h"
#include "UIItemView.h"

#include <SFML/Graphics.hpp>

#include <array>
#include <optional>
#include <vector>

namespace Roguelike
{
enum class HotbarUseState
{
    None,
    Empty,
    Used
};

struct HotbarUseResult
{
    HotbarUseState state = HotbarUseState::None;
    const ItemData* itemData = nullptr;
};

class HotbarPanel : public Engine::UIElement
{
   public:
    explicit HotbarPanel(const sf::Font& font);

    bool TryPlaceItem(sf::Vector2f mousePosition, const UIItemView& item);
    bool TryAutoPlaceItem(const ItemStack& item);
    bool ContainsPoint(sf::Vector2f mousePosition) const;
    void SetHighlightedItem(const std::optional<UIItemView>& item);
    void ClearHighlightedItem();
    HotbarUseResult TryUseHotkey(sf::Keyboard::Key key);
    void SetInventoryItems(const std::vector<ItemStack>& items);
    std::array<std::optional<ItemStack>, 6> GetSavedSlots() const;
    void SetSavedSlots(const std::array<std::optional<ItemStack>, 6>& savedSlots);

    void Update(float deltaTime) override;
    void Draw(sf::RenderWindow& window) override;

   private:
    void DrawSlot(sf::RenderWindow& window, int index);
    sf::FloatRect GetSlotBounds(int index) const;
    int GetSlotIndexForKey(sf::Keyboard::Key key) const;
    int GetInventoryCount(const ItemData* itemData) const;
    void RemoveMissingShortcuts();

   private:
    const sf::Font& font;

    // Six visible shortcuts mapped to keyboard keys 1..6. Counts come from inventoryItems.
    std::array<const ItemData*, 6> slots = {nullptr, nullptr, nullptr,
                                            nullptr, nullptr, nullptr};
    const std::vector<ItemStack>* inventoryItems = nullptr;
    std::optional<UIItemView> highlightedItem;

    // Per-slot pulse animation timers.
    std::array<float, 6> pulseTimers = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    // Hotbar row geometry in screen-space.
    sf::Vector2f position = {784.0f, 980.0f};

    float slotSize = 52.0f;
    float gap = 8.0f;
};
}  // namespace Roguelike
