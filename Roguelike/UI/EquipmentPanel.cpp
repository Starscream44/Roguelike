#include "pch.h"
#include "EquipmentPanel.h"

#include "ResourceSystem.h"
#include "UITextureUtils.h"

namespace Roguelike
{
namespace
{
// Equipment panel layout.
// Coordinates are local offsets from EquipmentPanel::position in EquipmentPanel.h.
// Change these values to move the avatar or individual equipment slots.
const sf::FloatRect AvatarBounds = {18.0f, 18.0f, 258.0f, 250.0f};

// Slot order must match EquipmentPanel::slotTypes:
// 0=head, 1=armor, 2=boots, 3=amulet, 4=weapon.
const std::array<sf::Vector2f, 5> EquipmentSlotOffsets = {{{356.0f, 60.0f},
                                                           {440.0f, 60.0f},
                                                           {356.0f, 144.0f},
                                                           {440.0f, 144.0f},
                                                           {620.0f, 102.0f}}};

// Item drawing inside each equipment slot. Bounds are local to the slot.
const sf::FloatRect EquipmentItemIconBounds = {12.0f, 12.0f, 52.0f, 52.0f};
const sf::Vector2f EquipmentFallbackIconPosition = {18.0f, 18.0f};
const sf::Vector2f EquipmentFallbackIconSize = {40.0f, 40.0f};

// Fallback avatar is only used if ui_player_avatar is missing.
const float FallbackAvatarHeadRadius = 28.0f;
const sf::Vector2f FallbackAvatarHeadPosition = {122.0f, 82.0f};
const sf::Vector2f FallbackAvatarBodyPosition = {108.0f, 146.0f};
const sf::Vector2f FallbackAvatarBodySize = {70.0f, 64.0f};
const sf::Vector2f FallbackAvatarBeltPosition = {104.0f, 190.0f};
const sf::Vector2f FallbackAvatarBeltSize = {78.0f, 10.0f};
}  // namespace

EquipmentPanel::EquipmentPanel(const sf::Font& uiFont) : FramedPanel(uiFont), font(uiFont)
{
    SetupFrame(position, size, "");
}

void EquipmentPanel::SetOpen(bool value)
{
    isOpen = value;

    if (isOpen)
    {
        Show();
    }
    else
    {
        Hide();
    }
}

bool EquipmentPanel::IsOpen() const { return isOpen; }

EquipmentPlacementPreview EquipmentPanel::PreviewPlacement(sf::Vector2f mousePosition,
                                                           const UIItemView& item) const
{
    EquipmentPlacementPreview preview;

    if (!isOpen || !CanEquip(item))
    {
        return preview;
    }

    for (int i = 0; i < static_cast<int>(slots.size()); ++i)
    {
        if (GetSlotBounds(i).contains(mousePosition))
        {
            preview.handled = true;
            preview.slotIndex = i;

            if (!CanEquipInSlot(item.stack, slotTypes[i]))
            {
                return preview;
            }

            preview.canPlace = true;
            if (slots[i].has_value())
            {
                preview.replacedItem = slots[i]->stack;
            }

            return preview;
        }
    }

    return preview;
}

void EquipmentPanel::CommitPlacement(const EquipmentPlacementPreview& preview,
                                     const UIItemView& item)
{
    if (!preview.canPlace || preview.slotIndex < 0 ||
        preview.slotIndex >= static_cast<int>(slots.size()))
    {
        return;
    }

    slots[preview.slotIndex] = UIItemView::FromStack(ItemStack{item.stack.data, 1});
}

bool EquipmentPanel::ContainsPoint(sf::Vector2f mousePosition) const
{
    if (!isOpen)
    {
        return false;
    }

    return IsPointOnAnySlot(mousePosition);
}

void EquipmentPanel::SetHighlightedItem(const std::optional<UIItemView>& item)
{
    highlightedItem = item;
}

void EquipmentPanel::ClearHighlightedItem()
{
    highlightedItem.reset();
}

std::array<std::optional<ItemStack>, 5> EquipmentPanel::GetSavedSlots() const
{
    std::array<std::optional<ItemStack>, 5> savedSlots;

    for (int i = 0; i < static_cast<int>(slots.size()); ++i)
    {
        if (slots[i].has_value())
        {
            savedSlots[i] = slots[i]->stack;
        }
    }

    return savedSlots;
}

void EquipmentPanel::SetSavedSlots(const std::array<std::optional<ItemStack>, 5>& savedSlots)
{
    for (int i = 0; i < static_cast<int>(slots.size()); ++i)
    {
        slots[i] = savedSlots[i].has_value() && savedSlots[i]->IsValid()
                       ? std::optional<UIItemView>(
                             UIItemView::FromStack(ItemStack{savedSlots[i]->data, 1}))
                       : std::nullopt;
    }
}

void EquipmentPanel::Draw(sf::RenderWindow& window)
{
    if (Engine::ResourceSystem::Instance()->HasTexture("ui_inventory_menu_top"))
    {
        DrawCharacterPreview(window);
        UITextureUtils::DrawTexture(window, "ui_inventory_menu_top",
                                    {position.x, position.y, size.x, size.y}, GetAlphaByte());
    }
    else
    {
        DrawFrame(window);
        DrawCharacterPreview(window);
    }

    for (int i = 0; i < static_cast<int>(slots.size()); ++i)
    {
        DrawSlot(window, i);
    }
}

void EquipmentPanel::DrawSlot(sf::RenderWindow& window, int index)
{
    sf::FloatRect bounds = GetSlotBounds(index);
    sf::Uint8 alpha = GetAlphaByte();
    const std::string textureKey =
        slotTypes[index] == EquipmentSlotType::Weapon ? "ui_slot_weapon" : "ui_slot_equipment";

    // Highlight only slots that can accept the currently dragged equipment item.
    bool isTargetSlot =
        highlightedItem.has_value() && CanEquipInSlot(highlightedItem->stack, slotTypes[index]);

    if (!UITextureUtils::DrawTexture(window, textureKey, bounds, alpha))
    {
        sf::RectangleShape slot;
        slot.setPosition({bounds.left, bounds.top});
        slot.setSize({bounds.width, bounds.height});
        slot.setFillColor(isTargetSlot ? sf::Color(72, 82, 48, alpha)
                                       : sf::Color(48, 64, 42, alpha));
        slot.setOutlineColor(isTargetSlot ? sf::Color(240, 216, 118, alpha)
                                          : sf::Color(116, 126, 72, alpha));
        slot.setOutlineThickness(isTargetSlot ? 4.0f : 2.0f);

        window.draw(slot);
    }

    if (isTargetSlot)
    {
        sf::RectangleShape highlight;
        highlight.setPosition({bounds.left, bounds.top});
        highlight.setSize({bounds.width, bounds.height});
        highlight.setFillColor(sf::Color::Transparent);
        highlight.setOutlineColor(sf::Color(240, 216, 118, alpha));
        highlight.setOutlineThickness(4.0f);

        window.draw(highlight);
    }

    if (!slots[index].has_value())
    {
        return;
    }

    const UIItemView& item = slots[index].value();

    if (!UITextureUtils::DrawItemTexture(
            window, item,
            {bounds.left + EquipmentItemIconBounds.left,
             bounds.top + EquipmentItemIconBounds.top, EquipmentItemIconBounds.width,
             EquipmentItemIconBounds.height},
            alpha))
    {
        sf::RectangleShape icon;
        icon.setPosition({bounds.left + EquipmentFallbackIconPosition.x,
                          bounds.top + EquipmentFallbackIconPosition.y});
        icon.setSize(EquipmentFallbackIconSize);

        sf::Color iconColor = item.iconColor;
        iconColor.a = alpha;
        icon.setFillColor(iconColor);

        window.draw(icon);
    }
}

void EquipmentPanel::DrawCharacterPreview(sf::RenderWindow& window)
{
    sf::Uint8 alpha = GetAlphaByte();

    bool hasTexturedBackground =
        Engine::ResourceSystem::Instance()->HasTexture("ui_inventory_menu_top");
    sf::FloatRect avatarBounds = {position.x + AvatarBounds.left, position.y + AvatarBounds.top,
                                  AvatarBounds.width, AvatarBounds.height};

    sf::RectangleShape previewFrame;
    previewFrame.setPosition({avatarBounds.left, avatarBounds.top});
    previewFrame.setSize({avatarBounds.width, avatarBounds.height});
    previewFrame.setFillColor(sf::Color(42, 58, 40, alpha));
    previewFrame.setOutlineColor(sf::Color(92, 108, 66, alpha));
    previewFrame.setOutlineThickness(2.0f);

    // The real avatar texture is preferred. If it is missing, draw a simple fallback preview
    // so the inventory UI still remains readable during asset changes.
    if (Engine::ResourceSystem::Instance()->HasTexture("ui_player_avatar"))
    {
        if (!hasTexturedBackground)
        {
            window.draw(previewFrame);
        }

        UITextureUtils::DrawTexture(window, "ui_player_avatar", avatarBounds, alpha);
        return;
    }

    sf::CircleShape head(FallbackAvatarHeadRadius);
    head.setPosition({position.x + FallbackAvatarHeadPosition.x,
                      position.y + FallbackAvatarHeadPosition.y});
    head.setFillColor(sf::Color(205, 170, 125, alpha));

    sf::RectangleShape body;
    body.setPosition({position.x + FallbackAvatarBodyPosition.x,
                      position.y + FallbackAvatarBodyPosition.y});
    body.setSize(FallbackAvatarBodySize);
    body.setFillColor(sf::Color(115, 80, 70, alpha));
    body.setOutlineColor(sf::Color(210, 190, 155, alpha));
    body.setOutlineThickness(2.0f);

    sf::RectangleShape belt;
    belt.setPosition({position.x + FallbackAvatarBeltPosition.x,
                      position.y + FallbackAvatarBeltPosition.y});
    belt.setSize(FallbackAvatarBeltSize);
    belt.setFillColor(sf::Color(70, 52, 48, alpha));

    if (!hasTexturedBackground)
    {
        window.draw(previewFrame);
    }

    window.draw(head);
    window.draw(body);
    window.draw(belt);
}

sf::FloatRect EquipmentPanel::GetSlotBounds(int index) const
{
    sf::Vector2f offset = EquipmentSlotOffsets[index];
    return {position.x + offset.x, position.y + offset.y, slotSize, slotSize};
}

bool EquipmentPanel::IsPointOnAnySlot(sf::Vector2f mousePosition) const
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
}  // namespace Roguelike
