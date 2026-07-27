#pragma once

#include "FramedPanel.h"
#include "UIItemView.h"

#include <SFML/Graphics.hpp>

#include <array>
#include <optional>
#include <string>

namespace Roguelike
{
struct EquipmentPlacementPreview
{
    bool handled = false;
    bool canPlace = false;
    int slotIndex = -1;
    std::optional<ItemStack> replacedItem;
};

class EquipmentPanel : public FramedPanel
{
   public:
    explicit EquipmentPanel(const sf::Font& font);

    void SetOpen(bool value);
    bool IsOpen() const;

    EquipmentPlacementPreview PreviewPlacement(sf::Vector2f mousePosition,
                                               const UIItemView& item) const;
    void CommitPlacement(const EquipmentPlacementPreview& preview, const UIItemView& item);
    bool ContainsPoint(sf::Vector2f mousePosition) const;
    void SetHighlightedItem(const std::optional<UIItemView>& item);
    void ClearHighlightedItem();
    std::array<std::optional<ItemStack>, 5> GetSavedSlots() const;
    void SetSavedSlots(const std::array<std::optional<ItemStack>, 5>& savedSlots);

    void Draw(sf::RenderWindow& window) override;

   private:
    void DrawSlot(sf::RenderWindow& window, int index);
    void DrawCharacterPreview(sf::RenderWindow& window);
    sf::FloatRect GetSlotBounds(int index) const;
    bool IsPointOnAnySlot(sf::Vector2f mousePosition) const;

   private:
    const sf::Font& font;

    // Equipment slots are drawn in this order and validated against slotTypes.
    std::array<std::optional<UIItemView>, 5> slots;
    std::optional<UIItemView> highlightedItem;
    std::array<EquipmentSlotType, 5> slotTypes = {EquipmentSlotType::Head,
                                                  EquipmentSlotType::Armor,
                                                  EquipmentSlotType::Boots,
                                                  EquipmentSlotType::Amulet,
                                                  EquipmentSlotType::Weapon};
    std::array<std::string, 5> slotLabels = {"Head", "Armor", "Boots", "Amulet", "Weapon"};

    bool isOpen = false;

    // Equipment panel geometry in screen-space.
    sf::Vector2f position = {580.0f, 90.0f};
    sf::Vector2f size = {760.0f, 280.0f};

    float slotSize = 76.0f;
};
}  // namespace Roguelike
