#include "pch.h"
#include "InventoryPanel.h"

#include "GameConfig.h"
#include "UITextureUtils.h"

#include <algorithm>

namespace Roguelike
{
namespace
{
// Inventory panel layout.
// Panel position, panel size, slot size, and slot gap live in InventoryPanel.h.
// Values below are local offsets from InventoryPanel::position.
const sf::Vector2f InventoryTitlePosition = {120.0f, 36.0f};

// Pager text positions. Move these together with the click bounds below.
const sf::Vector2f PagerPreviousPosition = {590.0f, 40.0f};
const sf::Vector2f PagerTextPosition = {628.0f, 44.0f};
const sf::Vector2f PagerNextPosition = {682.0f, 40.0f};

// Pager click areas. They are intentionally wider than the visible arrows.
const sf::FloatRect PagerPreviousClickBounds = {580.0f, 48.0f, 42.0f, 36.0f};
const sf::FloatRect PagerNextClickBounds = {682.0f, 48.0f, 42.0f, 36.0f};

// Top edge of the inventory slot grid, relative to the panel.
const float InventoryGridTop = 94.0f;
const float PageTransitionDuration = 0.22f;
const float PageSlideDistance = 34.0f;
const float ArrowPressDuration = 0.16f;
const float ArrowPressDistance = 5.0f;

// Item drawing inside a slot. Bounds are local to each slot.
const sf::FloatRect ItemIconBounds = {14.0f, 12.0f, 64.0f, 64.0f};
const sf::Vector2f ItemFallbackIconPosition = {18.0f, 16.0f};
const sf::Vector2f ItemFallbackIconSize = {56.0f, 56.0f};
const sf::Vector2f ItemCountOffsetFromBottomRight = {24.0f, 24.0f};

float Clamp01(float value)
{
    return std::clamp(value, 0.0f, 1.0f);
}

float SmoothStep(float value)
{
    float t = Clamp01(value);
    return t * t * (3.0f - 2.0f * t);
}

float RemainingPulse(float timer, float duration)
{
    if (duration <= 0.0f)
    {
        return 0.0f;
    }

    return SmoothStep(timer / duration);
}

sf::Uint8 ScaleAlpha(sf::Uint8 alpha, float scale)
{
    return static_cast<sf::Uint8>(std::clamp(static_cast<float>(alpha) * Clamp01(scale), 0.0f,
                                             255.0f));
}

sf::Color LerpColor(sf::Color from, sf::Color to, float amount, sf::Uint8 alpha)
{
    float t = Clamp01(amount);

    return sf::Color(
        static_cast<sf::Uint8>(static_cast<float>(from.r) +
                               (static_cast<float>(to.r) - static_cast<float>(from.r)) * t),
        static_cast<sf::Uint8>(static_cast<float>(from.g) +
                               (static_cast<float>(to.g) - static_cast<float>(from.g)) * t),
        static_cast<sf::Uint8>(static_cast<float>(from.b) +
                               (static_cast<float>(to.b) - static_cast<float>(from.b)) * t),
        alpha);
}
}  // namespace

InventoryPanel::InventoryPanel(const sf::Font& uiFont) : FramedPanel(uiFont), font(uiFont)
{
    SetupFrame(position, size, "Inventory");
}

void InventoryPanel::Toggle()
{
    isOpen = !isOpen;

    if (isOpen)
    {
        Show();
    }
    else
    {
        Hide();
        pageTransitionTimer = 0.0f;
        previousArrowPressTimer = 0.0f;
        nextArrowPressTimer = 0.0f;
    }
}

bool InventoryPanel::IsOpen() const { return isOpen; }

void InventoryPanel::SetItems(const std::vector<ItemStack>& newItems)
{
    items = &newItems;

    // Selection and page are clamped when inventory contents change after item use/pickup.
    if (selectedIndex >= static_cast<int>(items->size()))
    {
        selectedIndex = -1;
    }

    if (currentPage >= GameConfig::InventoryPages)
    {
        currentPage = GameConfig::InventoryPages - 1;
    }

    if (previousPage >= GameConfig::InventoryPages)
    {
        previousPage = currentPage;
    }
}

void InventoryPanel::ClearSelection() { selectedIndex = -1; }

bool InventoryPanel::TryChangePage(sf::Vector2f mousePosition)
{
    if (!isOpen)
    {
        return false;
    }

    if (GetPreviousPageBounds().contains(mousePosition))
    {
        ChangePage(-1);
        return true;
    }

    if (GetNextPageBounds().contains(mousePosition))
    {
        ChangePage(1);
        return true;
    }

    return false;
}

std::optional<UIItemView> InventoryPanel::TryPickItem(sf::Vector2f mousePosition)
{
    if (!isOpen)
    {
        return std::nullopt;
    }

    if (items == nullptr)
    {
        return std::nullopt;
    }

    int firstIndex = GetFirstPageItemIndex();
    int lastIndex = std::min(firstIndex + GetPageCapacity(), static_cast<int>(items->size()));

    for (int itemIndex = firstIndex; itemIndex < lastIndex; ++itemIndex)
    {
        int localIndex = itemIndex - firstIndex;
        if (GetSlotBounds(localIndex).contains(mousePosition))
        {
            selectedIndex = itemIndex;
            return UIItemView::FromStack((*items)[itemIndex]);
        }
    }

    return std::nullopt;
}

void InventoryPanel::Update(float deltaTime)
{
    FramedPanel::Update(deltaTime);

    pageTransitionTimer = std::max(0.0f, pageTransitionTimer - deltaTime);
    previousArrowPressTimer = std::max(0.0f, previousArrowPressTimer - deltaTime);
    nextArrowPressTimer = std::max(0.0f, nextArrowPressTimer - deltaTime);
}

void InventoryPanel::Draw(sf::RenderWindow& window)
{
    sf::Uint8 alpha = GetAlphaByte();

    if (!UITextureUtils::DrawTexture(window, "ui_inventory_menu_grid",
                                     {position.x, position.y, size.x, size.y}, alpha))
    {
        DrawFrame(window);
    }
    else
    {
        sf::Text titleText;
        titleText.setFont(font);
        titleText.setCharacterSize(26);
        titleText.setString("Inventory");
        titleText.setFillColor(sf::Color(226, 210, 132, alpha));
        titleText.setPosition(position + InventoryTitlePosition);

        window.draw(titleText);
    }

    DrawPageControls(window);

    if (pageTransitionTimer > 0.0f && pageTransitionDirection != 0)
    {
        float progress = 1.0f - pageTransitionTimer / PageTransitionDuration;
        float easedProgress = SmoothStep(progress);
        float direction = static_cast<float>(pageTransitionDirection);

        DrawInventoryPage(window, previousPage,
                          {-direction * PageSlideDistance * easedProgress, 0.0f},
                          ScaleAlpha(alpha, 1.0f - easedProgress));
        DrawInventoryPage(window, currentPage,
                          {direction * PageSlideDistance * (1.0f - easedProgress), 0.0f},
                          ScaleAlpha(alpha, easedProgress));
        return;
    }

    DrawInventoryPage(window, currentPage, {0.0f, 0.0f}, alpha);
}

void InventoryPanel::DrawPageControls(sf::RenderWindow& window)
{
    sf::Uint8 alpha = GetAlphaByte();
    float pagePulse = RemainingPulse(pageTransitionTimer, PageTransitionDuration);
    float previousPulse = RemainingPulse(previousArrowPressTimer, ArrowPressDuration);
    float nextPulse = RemainingPulse(nextArrowPressTimer, ArrowPressDuration);

    sf::Text pageText;
    pageText.setFont(font);
    pageText.setCharacterSize(16);
    pageText.setString(std::to_string(currentPage + 1) + "/" +
                       std::to_string(GameConfig::InventoryPages));
    pageText.setFillColor(LerpColor(sf::Color(216, 198, 118), sf::Color(255, 236, 154),
                                    pagePulse, alpha));
    pageText.setPosition(position + PagerTextPosition + sf::Vector2f{0.0f, -2.0f * pagePulse});

    sf::Text previousText;
    previousText.setFont(font);
    previousText.setCharacterSize(22);
    previousText.setString("<");
    previousText.setFillColor(LerpColor(sf::Color(232, 205, 116), sf::Color(255, 238, 160),
                                        previousPulse, alpha));
    previousText.setPosition(position + PagerPreviousPosition +
                             sf::Vector2f{-ArrowPressDistance * previousPulse, 0.0f});

    sf::Text nextText;
    nextText.setFont(font);
    nextText.setCharacterSize(22);
    nextText.setString(">");
    nextText.setFillColor(LerpColor(sf::Color(232, 205, 116), sf::Color(255, 238, 160),
                                    nextPulse, alpha));
    nextText.setPosition(position + PagerNextPosition +
                         sf::Vector2f{ArrowPressDistance * nextPulse, 0.0f});

    window.draw(pageText);
    window.draw(previousText);
    window.draw(nextText);
}

void InventoryPanel::DrawInventoryPage(sf::RenderWindow& window, int page, sf::Vector2f offset,
                                       sf::Uint8 pageAlpha)
{
    if (pageAlpha == 0)
    {
        return;
    }

    for (int localIndex = 0; localIndex < GetPageCapacity(); ++localIndex)
    {
        DrawSlot(window, localIndex, offset, pageAlpha);
    }

    if (items == nullptr || items->empty())
    {
        DrawEmptyText(window, offset, pageAlpha);
        return;
    }

    int firstIndex = GetFirstPageItemIndex(page);
    int lastIndex = std::min(firstIndex + GetPageCapacity(), static_cast<int>(items->size()));

    for (int itemIndex = firstIndex; itemIndex < lastIndex; ++itemIndex)
    {
        DrawItem(window, UIItemView::FromStack((*items)[itemIndex]), itemIndex - firstIndex,
                 itemIndex, offset, pageAlpha);
    }
}

void InventoryPanel::DrawEmptyText(sf::RenderWindow& window, sf::Vector2f offset,
                                   sf::Uint8 pageAlpha)
{
    sf::Text emptyText;
    emptyText.setFont(font);
    emptyText.setCharacterSize(18);
    emptyText.setString("No items yet");
    emptyText.setFillColor(sf::Color(205, 198, 130, pageAlpha));
    emptyText.setPosition({position.x + 68.0f + offset.x, position.y + size.y - 56.0f + offset.y});

    window.draw(emptyText);
}

void InventoryPanel::DrawSlot(sf::RenderWindow& window, int localIndex, sf::Vector2f offset,
                              sf::Uint8 pageAlpha)
{
    sf::FloatRect bounds = GetSlotBounds(localIndex, offset);

    if (UITextureUtils::DrawTexture(window, "ui_slot_inventory", bounds, pageAlpha))
    {
        return;
    }

    sf::RectangleShape slot;
    slot.setPosition({bounds.left, bounds.top});
    slot.setSize({bounds.width, bounds.height});
    slot.setFillColor(sf::Color(48, 64, 42, pageAlpha));
    slot.setOutlineColor(sf::Color(104, 118, 66, pageAlpha));
    slot.setOutlineThickness(2.0f);
    window.draw(slot);
}

void InventoryPanel::DrawItem(sf::RenderWindow& window, const UIItemView& item, int localIndex,
                              int itemIndex, sf::Vector2f offset, sf::Uint8 pageAlpha)
{
    sf::FloatRect bounds = GetSlotBounds(localIndex, offset);

    if (itemIndex == selectedIndex)
    {
        sf::RectangleShape selection;
        selection.setPosition({bounds.left, bounds.top});
        selection.setSize({bounds.width, bounds.height});
        selection.setFillColor(sf::Color::Transparent);
        selection.setOutlineColor(sf::Color(236, 214, 126, pageAlpha));
        selection.setOutlineThickness(4.0f);

        window.draw(selection);
    }

    bool drewItemTexture = UITextureUtils::DrawItemTexture(
        window, item,
        {bounds.left + ItemIconBounds.left, bounds.top + ItemIconBounds.top,
         ItemIconBounds.width, ItemIconBounds.height},
        pageAlpha);

    sf::Text countText;
    countText.setFont(font);
    countText.setCharacterSize(13);
    countText.setString(std::to_string(item.stack.count));
    countText.setFillColor(sf::Color(238, 214, 142, pageAlpha));
    countText.setPosition({bounds.left + bounds.width - ItemCountOffsetFromBottomRight.x,
                           bounds.top + bounds.height - ItemCountOffsetFromBottomRight.y});

    if (!drewItemTexture)
    {
        sf::RectangleShape icon;
        icon.setPosition({bounds.left + ItemFallbackIconPosition.x,
                          bounds.top + ItemFallbackIconPosition.y});
        icon.setSize(ItemFallbackIconSize);

        sf::Color iconColor = item.iconColor;
        iconColor.a = pageAlpha;
        icon.setFillColor(iconColor);

        window.draw(icon);
    }

    window.draw(countText);
}

sf::FloatRect InventoryPanel::GetSlotBounds(int localIndex, sf::Vector2f offset) const
{
    int row = localIndex / GameConfig::InventoryColumns;
    int column = localIndex % GameConfig::InventoryColumns;

    const float gridWidth =
        static_cast<float>(GameConfig::InventoryColumns) * slotSize +
        static_cast<float>(GameConfig::InventoryColumns - 1) * gap;
    const float startX = position.x + (size.x - gridWidth) * 0.5f;

    float x = startX + static_cast<float>(column) * (slotSize + gap) + offset.x;
    float y = position.y + InventoryGridTop + static_cast<float>(row) * (slotSize + gap) + offset.y;

    return {x, y, slotSize, slotSize};
}

sf::FloatRect InventoryPanel::GetPreviousPageBounds() const
{
    return {position.x + PagerPreviousClickBounds.left, position.y + PagerPreviousClickBounds.top,
            PagerPreviousClickBounds.width, PagerPreviousClickBounds.height};
}

sf::FloatRect InventoryPanel::GetNextPageBounds() const
{
    return {position.x + PagerNextClickBounds.left, position.y + PagerNextClickBounds.top,
            PagerNextClickBounds.width, PagerNextClickBounds.height};
}

int InventoryPanel::GetFirstPageItemIndex() const
{
    return GetFirstPageItemIndex(currentPage);
}

int InventoryPanel::GetFirstPageItemIndex(int page) const
{
    return page * GetPageCapacity();
}

int InventoryPanel::GetPageCapacity() const
{
    return GameConfig::InventoryColumns * GameConfig::InventoryRows;
}

void InventoryPanel::ChangePage(int direction)
{
    if (GameConfig::InventoryPages <= 1 || direction == 0)
    {
        return;
    }

    previousPage = currentPage;
    currentPage =
        (currentPage + GameConfig::InventoryPages + direction) % GameConfig::InventoryPages;
    pageTransitionDirection = direction < 0 ? -1 : 1;
    pageTransitionTimer = PageTransitionDuration;

    if (direction < 0)
    {
        previousArrowPressTimer = ArrowPressDuration;
    }
    else
    {
        nextArrowPressTimer = ArrowPressDuration;
    }

    ClearSelection();
}
}  // namespace Roguelike
