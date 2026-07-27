#pragma once

#include "FramedPanel.h"
#include "UIItemView.h"

#include <SFML/Graphics.hpp>

#include <optional>
#include <vector>

namespace Roguelike
{
class InventoryPanel : public FramedPanel
{
   public:
    explicit InventoryPanel(const sf::Font& font);

    void Toggle();
    bool IsOpen() const;

    void SetItems(const std::vector<ItemStack>& newItems);
    void ClearSelection();
    bool TryChangePage(sf::Vector2f mousePosition);
    std::optional<UIItemView> TryPickItem(sf::Vector2f mousePosition);

    void Update(float deltaTime) override;
    void Draw(sf::RenderWindow& window) override;

   private:
    void DrawPageControls(sf::RenderWindow& window);
    void DrawInventoryPage(sf::RenderWindow& window, int page, sf::Vector2f offset,
                           sf::Uint8 pageAlpha);
    void DrawEmptyText(sf::RenderWindow& window, sf::Vector2f offset, sf::Uint8 pageAlpha);
    void DrawSlot(sf::RenderWindow& window, int localIndex, sf::Vector2f offset,
                  sf::Uint8 pageAlpha);
    void DrawItem(sf::RenderWindow& window, const UIItemView& item, int localIndex, int itemIndex,
                  sf::Vector2f offset, sf::Uint8 pageAlpha);
    sf::FloatRect GetSlotBounds(int localIndex, sf::Vector2f offset = {0.0f, 0.0f}) const;
    sf::FloatRect GetPreviousPageBounds() const;
    sf::FloatRect GetNextPageBounds() const;
    int GetFirstPageItemIndex() const;
    int GetFirstPageItemIndex(int page) const;
    int GetPageCapacity() const;
    void ChangePage(int direction);

   private:
    const sf::Font& font;
    const std::vector<ItemStack>* items = nullptr;

    // Selection is an absolute inventory index; page is zero-based.
    int selectedIndex = -1;
    int currentPage = 0;

    // Pager animation state is purely visual. Inventory data changes immediately,
    // while Draw() briefly uses previousPage to make that change readable.
    int previousPage = 0;
    int pageTransitionDirection = 0;
    float pageTransitionTimer = 0.0f;
    float previousArrowPressTimer = 0.0f;
    float nextArrowPressTimer = 0.0f;

    bool isOpen = false;

    // Main inventory panel geometry in screen-space.
    sf::Vector2f position = {580.0f, 370.0f};
    sf::Vector2f size = {760.0f, 350.0f};

    // Slot grid tuning. Column/row count comes from GameConfig.
    float slotSize = 92.0f;
    float gap = 18.0f;
};
}  // namespace Roguelike
