#include "pch.h"
#include "HotbarPanel.h"

#include "UITextureUtils.h"

#include <algorithm>

namespace Roguelike
{
namespace
{
// Hotbar frame layout.
// Panel position, slot size, and gap live in HotbarPanel.h.
// Padding expands the decorative frame around the computed slot row.
const sf::Vector2f HotbarFramePadding = {28.0f, 18.0f};
const float HotbarFrameHeight = 88.0f;

// Per-slot text/icon layout. These offsets are local to the slot bounds.
const sf::FloatRect HotbarItemIconBounds = {10.0f, 10.0f, 32.0f, 32.0f};
const sf::Vector2f HotbarCountPosition = {34.0f, 34.0f};
const sf::Vector2f HotbarNumberPosition = {4.0f, 2.0f};

// Hotkey feedback animation. Duration controls how long the slot pulse lasts;
// inflate controls how far the slot grows during that pulse.
const float HotbarPulseSeconds = 0.18f;
const float HotbarPulseInflate = 3.0f;
}  // namespace

HotbarPanel::HotbarPanel(const sf::Font& uiFont) : font(uiFont) {}

bool HotbarPanel::TryPlaceItem(sf::Vector2f mousePosition, const UIItemView& item)
{
    if (!CanUseOnHotbar(item))
    {
        return false;
    }

    for (const ItemData*& slot : slots)
    {
        if (slot == item.stack.data)
        {
            slot = nullptr;
        }
    }

    for (int i = 0; i < static_cast<int>(slots.size()); ++i)
    {
        if (GetSlotBounds(i).contains(mousePosition))
        {
            slots[i] = item.stack.data;
            return true;
        }
    }

    return false;
}

bool HotbarPanel::TryAutoPlaceItem(const ItemStack& item)
{
    if (!CanUseOnHotbar(item))
    {
        return false;
    }

    for (const ItemData* slot : slots)
    {
        if (slot == item.data)
        {
            return true;
        }
    }

    for (const ItemData*& slot : slots)
    {
        if (slot == nullptr)
        {
            slot = item.data;
            return true;
        }
    }

    return false;
}

bool HotbarPanel::ContainsPoint(sf::Vector2f mousePosition) const
{
    for (int i = 0; i < static_cast<int>(slots.size()); ++i)
    {
        if (GetSlotBounds(i).contains(mousePosition))
        {
            return true;
        }
    }

    return false;
}

void HotbarPanel::SetHighlightedItem(const std::optional<UIItemView>& item)
{
    highlightedItem = item;
}

void HotbarPanel::ClearHighlightedItem()
{
    highlightedItem.reset();
}

HotbarUseResult HotbarPanel::TryUseHotkey(sf::Keyboard::Key key)
{
    int slotIndex = GetSlotIndexForKey(key);

    if (slotIndex < 0)
    {
        return {};
    }

    pulseTimers[slotIndex] = HotbarPulseSeconds;

    const ItemData* itemData = slots[slotIndex];
    if (itemData == nullptr || GetInventoryCount(itemData) <= 0)
    {
        slots[slotIndex] = nullptr;
        return {HotbarUseState::Empty, nullptr};
    }

    return {HotbarUseState::Used, itemData};
}

void HotbarPanel::SetInventoryItems(const std::vector<ItemStack>& items)
{
    inventoryItems = &items;
    RemoveMissingShortcuts();
}

std::array<std::optional<ItemStack>, 6> HotbarPanel::GetSavedSlots() const
{
    std::array<std::optional<ItemStack>, 6> savedSlots;

    for (int i = 0; i < static_cast<int>(slots.size()); ++i)
    {
        if (slots[i] != nullptr)
        {
            savedSlots[i] = ItemStack{slots[i], 1};
        }
    }

    return savedSlots;
}

void HotbarPanel::SetSavedSlots(const std::array<std::optional<ItemStack>, 6>& savedSlots)
{
    for (int i = 0; i < static_cast<int>(slots.size()); ++i)
    {
        slots[i] = savedSlots[i].has_value() && savedSlots[i]->IsValid() ? savedSlots[i]->data
                                                                         : nullptr;
    }

    RemoveMissingShortcuts();
}

void HotbarPanel::Update(float deltaTime)
{
    Engine::UIElement::Update(deltaTime);

    for (float& timer : pulseTimers)
    {
        timer = std::max(0.0f, timer - deltaTime);
    }
}

void HotbarPanel::Draw(sf::RenderWindow& window)
{
    const float totalWidth = static_cast<float>(slots.size()) * slotSize +
                             static_cast<float>(slots.size() - 1) * gap;
    UITextureUtils::DrawTexture(window, "ui_hotbar_frame",
                                {position.x - HotbarFramePadding.x,
                                 position.y - HotbarFramePadding.y,
                                 totalWidth + HotbarFramePadding.x * 2.0f, HotbarFrameHeight},
                                GetAlphaByte());

    for (int i = 0; i < static_cast<int>(slots.size()); ++i)
    {
        DrawSlot(window, i);
    }
}

void HotbarPanel::DrawSlot(sf::RenderWindow& window, int index)
{
    sf::FloatRect bounds = GetSlotBounds(index);
    sf::Uint8 alpha = GetAlphaByte();
    // Pulse gently enlarges the slot after hotkey use. Highlight is separate
    // and shows valid drag-and-drop targets while the inventory is open.
    float pulse = pulseTimers[index] / HotbarPulseSeconds;
    float inflate = HotbarPulseInflate * pulse;
    bool isTargetSlot = highlightedItem.has_value() && CanUseOnHotbar(highlightedItem->stack) &&
                        (slots[index] == nullptr || slots[index] == highlightedItem->stack.data);

    bool drewSlotTexture =
        UITextureUtils::DrawTexture(window, "ui_slot_hotbar",
                                    {bounds.left - inflate, bounds.top - inflate,
                                     bounds.width + inflate * 2.0f,
                                     bounds.height + inflate * 2.0f},
                                    alpha);

    if (!drewSlotTexture)
    {
        sf::RectangleShape slot;
        slot.setPosition({bounds.left - inflate, bounds.top - inflate});
        slot.setSize({bounds.width + inflate * 2.0f, bounds.height + inflate * 2.0f});
        slot.setFillColor(isTargetSlot
                              ? sf::Color(68, 58, 35, alpha)
                              : sf::Color(45 + static_cast<sf::Uint8>(35.0f * pulse), 36, 32,
                                          alpha));
        slot.setOutlineColor((pulse > 0.0f || isTargetSlot) ? sf::Color(245, 225, 130, alpha)
                                                            : sf::Color(190, 150, 90, alpha));
        slot.setOutlineThickness((isTargetSlot ? 4.0f : 2.0f) + 2.0f * pulse);

        window.draw(slot);
    }

    if (drewSlotTexture && (pulse > 0.0f || isTargetSlot))
    {
        sf::RectangleShape highlight;
        highlight.setPosition({bounds.left - inflate, bounds.top - inflate});
        highlight.setSize({bounds.width + inflate * 2.0f, bounds.height + inflate * 2.0f});
        highlight.setFillColor(sf::Color::Transparent);
        highlight.setOutlineColor(sf::Color(245, 225, 130, alpha));
        highlight.setOutlineThickness((isTargetSlot ? 4.0f : 2.0f) + 2.0f * pulse);

        window.draw(highlight);
    }

    sf::Text numberText;
    numberText.setFont(font);
    numberText.setCharacterSize(12);
    numberText.setString(std::to_string(index + 1));
    numberText.setFillColor(sf::Color(230, 220, 190, alpha));
    numberText.setPosition(bounds.left + HotbarNumberPosition.x,
                           bounds.top + HotbarNumberPosition.y);

    window.draw(numberText);

    const ItemData* itemData = slots[index];
    int itemCount = GetInventoryCount(itemData);

    if (itemData == nullptr || itemCount <= 0)
    {
        return;
    }

    UIItemView item = UIItemView::FromStack(ItemStack{itemData, itemCount});

    if (!UITextureUtils::DrawItemTexture(window, item,
                                         {bounds.left + HotbarItemIconBounds.left,
                                          bounds.top + HotbarItemIconBounds.top,
                                          HotbarItemIconBounds.width,
                                          HotbarItemIconBounds.height},
                                         alpha))
    {
        sf::RectangleShape icon;
        icon.setPosition({bounds.left + 13.0f, bounds.top + 14.0f});
        icon.setSize({26.0f, 26.0f});

        sf::Color iconColor = item.iconColor;
        iconColor.a = alpha;
        icon.setFillColor(iconColor);

        window.draw(icon);
    }

    sf::Text countText;
    countText.setFont(font);
    countText.setCharacterSize(12);
    countText.setString(std::to_string(itemCount));
    countText.setFillColor(sf::Color(255, 255, 255, alpha));
    countText.setPosition(bounds.left + HotbarCountPosition.x, bounds.top + HotbarCountPosition.y);

    window.draw(countText);
}

sf::FloatRect HotbarPanel::GetSlotBounds(int index) const
{
    float x = position.x + static_cast<float>(index) * (slotSize + gap);
    float y = position.y;

    return {x, y, slotSize, slotSize};
}

int HotbarPanel::GetSlotIndexForKey(sf::Keyboard::Key key) const
{
    static const std::array<sf::Keyboard::Key, 6> keys = {sf::Keyboard::Num1, sf::Keyboard::Num2,
                                                          sf::Keyboard::Num3, sf::Keyboard::Num4,
                                                          sf::Keyboard::Num5, sf::Keyboard::Num6};

    for (int i = 0; i < static_cast<int>(keys.size()); ++i)
    {
        if (keys[i] == key)
        {
            return i;
        }
    }

    return -1;
}

int HotbarPanel::GetInventoryCount(const ItemData* itemData) const
{
    if (itemData == nullptr || inventoryItems == nullptr)
    {
        return 0;
    }

    for (const ItemStack& item : *inventoryItems)
    {
        if (item.data == itemData)
        {
            return item.count;
        }
    }

    return 0;
}

void HotbarPanel::RemoveMissingShortcuts()
{
    if (inventoryItems == nullptr)
    {
        return;
    }

    for (const ItemData*& slot : slots)
    {
        if (slot != nullptr && GetInventoryCount(slot) <= 0)
        {
            slot = nullptr;
        }
    }
}
}  // namespace Roguelike
