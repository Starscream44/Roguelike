#include "pch.h"
#include "GameUIComponent.h"

#include "Engine.h"
#include "GameConfig.h"
#include "GameNotifications.h"
#include "GameWorld.h"
#include "LevelProgress.h"
#include "PlayerMovementComponent.h"
#include "PlayerRunSnapshot.h"
#include "RenderSystem.h"
#include "SaveSystem.h"
#include "StatsComponent.h"
#include "UITextureUtils.h"

#include <algorithm>

namespace Roguelike
{
namespace
{
// Dragged item cursor visual tuning.
const float DraggedItemTextureSize = 44.0f;
const float DraggedItemFallbackSize = 32.0f;
const sf::Uint8 DraggedItemTextureAlpha = 230;
const sf::Uint8 DraggedItemFallbackAlpha = 220;

// Popup feedback durations.
const float LevelPopupSeconds = 2.0f;
const float TutorialPopupSeconds = 3.0f;
const float QuickFeedbackSeconds = 1.5f;

float ClampPlayerArmor(float armor)
{
    return std::clamp(armor, 0.0f, GameConfig::MaxPlayerArmor);
}
}  // namespace

GameUIComponent::GameUIComponent(Engine::GameObject* gameObject) : Component(gameObject)
{
    font.loadFromFile(GameConfig::MainUIFontPath);
    titleFont.loadFromFile(GameConfig::TitleFontPath);
    CreateUI();

    Engine::GameWorld::Instance()->SetPaused(true);
}

void GameUIComponent::SetPlayer(Engine::GameObject* player)
{
    playerObject = player;
    playerInventory =
        playerObject != nullptr ? playerObject->GetComponent<InventoryComponent>() : nullptr;

    Engine::StatsComponent* stats =
        playerObject != nullptr ? playerObject->GetComponent<Engine::StatsComponent>() : nullptr;

    if (stats != nullptr)
    {
        // Stats drive HUD updates by event; the frame update only refreshes objective text.
        stats->AddStatsChangedListener(
            [this](float health, float armor) { UpdateHUDStats(health, armor); });
        UpdateHUDStats(stats->GetHealth(), stats->GetArmor());
    }

    RestorePlayerRunState();
}

void GameUIComponent::SetLevelObjective(const std::vector<Engine::GameObject*>& enemies, int level)
{
    objectiveEnemies = enemies;
    levelNumber = level;
    totalEnemyCount = static_cast<int>(objectiveEnemies.size());
    aliveEnemyCount = totalEnemyCount;
    isLevelComplete = false;

    if (levelNumber > 1)
    {
        isMainMenuOpen = false;

        if (overlay != nullptr)
        {
            overlay->HideOverlay();
        }

        Engine::GameWorld::Instance()->SetPaused(false);
        ShowPopupMessage("Level " + std::to_string(levelNumber), LevelPopupSeconds);
    }
}

void GameUIComponent::HandleEvent(const sf::Event& event)
{
    uiManager.HandleEvent(event);

    if (event.type == sf::Event::KeyPressed)
    {
        HandleKeyPressed(event.key.code);
        return;
    }

    if (event.type == sf::Event::MouseButtonPressed)
    {
        sf::RenderWindow& window = Engine::RenderSystem::Instance()->GetMainWindow();
        HandleMouseButtonPressed(event.mouseButton, window);
    }
}

void GameUIComponent::Update(float deltaTime)
{
    UpdateLevelObjective(deltaTime);
    HandleDeathState();
    UpdateHUD();
    UpdateInventory();

    std::optional<std::string> notification = GameNotifications::Consume();
    if (notification.has_value())
    {
        ShowPopupMessage(notification.value());
    }

    uiManager.Update(deltaTime);
}

void GameUIComponent::Render()
{
    sf::RenderWindow& window = Engine::RenderSystem::Instance()->GetMainWindow();

    sf::View oldView = window.getView();
    window.setView(window.getDefaultView());

    uiManager.Draw(window);
    DrawDraggedItem(window);

    window.setView(oldView);
}

void GameUIComponent::CreateUI()
{
    hud = &uiManager.CreateElement<HUD>(font, titleFont);
    hotbar = &uiManager.CreateElement<HotbarPanel>(font);
    inventory = &uiManager.CreateElement<InventoryPanel>(font);
    equipment = &uiManager.CreateElement<EquipmentPanel>(font);
    descriptionPanel = &uiManager.CreateElement<ItemDescriptionPanel>(font);
    popup = &uiManager.CreateElement<PopupMessage>(font);
    overlay = &uiManager.CreateElement<GameScreenOverlay>(font, titleFont);

    overlay->ShowMainMenu();
}

void GameUIComponent::UpdateHUD()
{
    if (hud == nullptr)
    {
        return;
    }

    hud->SetObjective(levelNumber, aliveEnemyCount, totalEnemyCount);
}

void GameUIComponent::UpdateHUDStats(float health, float armor)
{
    if (hud == nullptr)
    {
        return;
    }

    hud->SetStats(health, GameConfig::PlayerHealth, ClampPlayerArmor(armor),
                  GameConfig::MaxPlayerArmor);
}

void GameUIComponent::UpdateLevelObjective(float deltaTime)
{
    if (isMainMenuOpen || isPauseOpen || isGameOver || isLevelComplete)
    {
        return;
    }

    aliveEnemyCount = 0;

    for (Engine::GameObject* enemy : objectiveEnemies)
    {
        if (Engine::GameWorld::Instance()->IsGameObjectAlive(enemy))
        {
            aliveEnemyCount++;
        }
    }

    if (aliveEnemyCount > 0 || totalEnemyCount <= 0)
    {
        return;
    }

    isLevelComplete = true;
    CloseInventory();

    if (overlay != nullptr)
    {
        overlay->ShowLevelCleared(levelNumber + 1);
    }

    Engine::GameWorld::Instance()->SetPaused(true);
}

void GameUIComponent::UpdateInventory()
{
    if (inventory == nullptr || playerInventory == nullptr)
    {
        return;
    }

    const std::vector<ItemStack>& items = playerInventory->GetItems();
    inventory->SetItems(items);

    if (hotbar != nullptr)
    {
        hotbar->SetInventoryItems(items);
    }

    AutoPlaceNewHotbarItems(items);
}

void GameUIComponent::AutoPlaceNewHotbarItems(const std::vector<ItemStack>& items)
{
    if (hotbar == nullptr)
    {
        knownInventoryItems = items;
        return;
    }

    for (const ItemStack& item : items)
    {
        if (!CanUseOnHotbar(item) || item.count <= GetKnownItemCount(item.data))
        {
            continue;
        }

        hotbar->TryAutoPlaceItem(item);
    }

    knownInventoryItems = items;
}

int GameUIComponent::GetKnownItemCount(const ItemData* itemData) const
{
    for (const ItemStack& item : knownInventoryItems)
    {
        if (item.data == itemData)
        {
            return item.count;
        }
    }

    return 0;
}

void GameUIComponent::HandleKeyPressed(sf::Keyboard::Key key)
{
    bool wasGameOver = isGameOver;
    HandleDeathState();
    if (!wasGameOver && isGameOver)
    {
        return;
    }

    // Modal screens consume keys before gameplay-facing UI can react.
    if (isGameOver)
    {
        if (key == sf::Keyboard::Space)
        {
            HandleGameOverRestartPressed();
        }
        return;
    }

    if (isLevelComplete)
    {
        if (key == sf::Keyboard::Space)
        {
            HandleLevelCompleteNextPressed();
        }
        return;
    }

    if (isMainMenuOpen)
    {
        if (key == sf::Keyboard::Space)
        {
            HandleStartPressed();
        }
        return;
    }

    if (key == sf::Keyboard::Escape)
    {
        HandlePausePressed();
        return;
    }

    if (isPauseOpen)
    {
        return;
    }

    if (key == sf::Keyboard::I)
    {
        HandleInventoryTogglePressed();
        return;
    }

    HandleHotbarKey(key);
}

void GameUIComponent::HandleMouseButtonPressed(const sf::Event::MouseButtonEvent& mouseButton,
                                               sf::RenderWindow& window)
{
    if (isMainMenuOpen || isPauseOpen || isGameOver || isLevelComplete ||
        inventory == nullptr || !inventory->IsOpen())
    {
        return;
    }

    sf::Vector2f mousePosition =
        window.mapPixelToCoords({mouseButton.x, mouseButton.y}, window.getDefaultView());

    if (mouseButton.button == sf::Mouse::Right)
    {
        HandleInventoryCancelPressed();
        return;
    }

    if (mouseButton.button == sf::Mouse::Left)
    {
        HandleInventoryClick(mousePosition);
    }
}

void GameUIComponent::HandleStartPressed()
{
    isMainMenuOpen = false;

    if (overlay != nullptr)
    {
        overlay->HideOverlay();
    }

    Engine::GameWorld::Instance()->SetPaused(false);

    if (popup != nullptr)
    {
        popup->ShowMessage("Press I to open inventory", TutorialPopupSeconds);
    }
}

void GameUIComponent::HandlePausePressed()
{
    if (inventory != nullptr && inventory->IsOpen())
    {
        CloseInventory();
        Engine::GameWorld::Instance()->SetPaused(false);
        return;
    }

    isPauseOpen = !isPauseOpen;

    if (isPauseOpen)
    {
        if (overlay != nullptr)
        {
            overlay->ShowPause();
        }

        Engine::GameWorld::Instance()->SetPaused(true);
        return;
    }

    if (overlay != nullptr)
    {
        overlay->HideOverlay();
    }

    Engine::GameWorld::Instance()->SetPaused(false);
}

void GameUIComponent::HandleInventoryTogglePressed()
{
    if (inventory == nullptr)
    {
        return;
    }

    ToggleInventory();
    Engine::GameWorld::Instance()->SetPaused(inventory->IsOpen());
}

void GameUIComponent::HandleInventoryClick(sf::Vector2f mousePosition)
{
    if (inventory == nullptr)
    {
        return;
    }

    if (inventory->TryChangePage(mousePosition))
    {
        return;
    }

    std::optional<UIItemView> selectedItem = inventory->TryPickItem(mousePosition);

    if (selectedItem.has_value())
    {
        SelectItem(selectedItem.value());
        return;
    }

    if (TryPlaceSelectedItem(mousePosition) || draggedItem.has_value())
    {
        ClearSelectedItem();
    }
}

void GameUIComponent::HandleInventoryCancelPressed()
{
    if (draggedItem.has_value())
    {
        // Right click cancels drag-and-drop without touching inventory data.
        ClearSelectedItem();
    }
}

void GameUIComponent::HandleHotbarKey(sf::Keyboard::Key key)
{
    if (hotbar == nullptr || popup == nullptr)
    {
        return;
    }

    HotbarUseResult useResult = hotbar->TryUseHotkey(key);

    if (useResult.state == HotbarUseState::None)
    {
        return;
    }

    if (useResult.state == HotbarUseState::Empty || useResult.itemData == nullptr)
    {
        popup->ShowMessage("Empty slot", QuickFeedbackSeconds);
        return;
    }

    std::string effectMessage = ApplyHotbarItemEffect(*useResult.itemData);

    if (playerInventory != nullptr)
    {
        playerInventory->RemoveOneItem(useResult.itemData);
    }

    popup->ShowMessage(effectMessage, QuickFeedbackSeconds);
}

void GameUIComponent::HandleDeathState()
{
    if (isGameOver || playerObject == nullptr)
    {
        return;
    }

    Engine::StatsComponent* stats = playerObject->GetComponent<Engine::StatsComponent>();

    if (stats == nullptr || !stats->IsDead())
    {
        return;
    }

    isGameOver = true;

    if (inventory != nullptr && inventory->IsOpen())
    {
        CloseInventory();
    }

    ClearSelectedItem();

    if (overlay != nullptr)
    {
        overlay->ShowGameOver();
    }

    Engine::GameWorld::Instance()->SetPaused(true);
}

void GameUIComponent::HandleGameOverRestartPressed()
{
    Engine::SaveSystem::Instance()->RemoveValue(PlayerRunSnapshotKey);
    LevelProgress::Reset();
    Engine::Engine::Instance()->RequestSceneRestart();
}

void GameUIComponent::HandleLevelCompleteNextPressed()
{
    SavePlayerRunState();
    LevelProgress::Advance();
    Engine::Engine::Instance()->RequestSceneRestart();
}

std::string GameUIComponent::ApplyHotbarItemEffect(const ItemData& itemData)
{
    Engine::StatsComponent* stats =
        playerObject != nullptr ? playerObject->GetComponent<Engine::StatsComponent>() : nullptr;

    if (itemData.effectType == ItemEffectType::RestoreHealth && stats != nullptr)
    {
        float restoredHealth =
            std::min(GameConfig::PlayerHealth, stats->GetHealth() + itemData.effectAmount);
        stats->SetStats(restoredHealth, ClampPlayerArmor(stats->GetArmor()));
        return "Health restored";
    }

    if (itemData.effectType == ItemEffectType::IncreaseAttack && stats != nullptr)
    {
        stats->SetAttackPower(stats->GetAttackPower() + itemData.effectAmount);
        return "Attack increased";
    }

    if (itemData.effectType == ItemEffectType::IncreaseSpeed && playerObject != nullptr)
    {
        PlayerMovementComponent* movement = playerObject->GetComponent<PlayerMovementComponent>();

        if (movement != nullptr)
        {
            movement->SetSpeed(
                std::min(GameConfig::MaxPlayerMoveSpeed,
                         movement->GetSpeed() + itemData.effectAmount));
            return "Speed increased";
        }
    }

    return "Used: " + std::string(itemData.name);
}

void GameUIComponent::ApplyEquipmentChange(const ItemData* equippedItem,
                                           const ItemData* replacedItem)
{
    ApplyEquipmentBonuses(replacedItem, -1.0f);
    ApplyEquipmentBonuses(equippedItem, 1.0f);
}

void GameUIComponent::ApplyEquipmentBonuses(const ItemData* itemData, float direction)
{
    if (itemData == nullptr || playerObject == nullptr)
    {
        return;
    }

    Engine::StatsComponent* stats = playerObject->GetComponent<Engine::StatsComponent>();
    if (stats != nullptr)
    {
        if (itemData->armorBonus != 0.0f)
        {
            stats->SetStats(stats->GetHealth(),
                            ClampPlayerArmor(stats->GetArmor() +
                                             itemData->armorBonus * direction));
        }

        if (itemData->attackBonus != 0.0f)
        {
            stats->SetAttackPower(std::max(0.0f, stats->GetAttackPower() +
                                                     itemData->attackBonus * direction));
        }
    }

    if (itemData->speedBonus != 0.0f)
    {
        PlayerMovementComponent* movement = playerObject->GetComponent<PlayerMovementComponent>();

        if (movement != nullptr)
        {
            movement->SetSpeed(std::clamp(movement->GetSpeed() +
                                              itemData->speedBonus * direction,
                                          0.0f, GameConfig::MaxPlayerMoveSpeed));
        }
    }
}

void GameUIComponent::RestorePlayerRunState()
{
    if (playerInventory == nullptr)
    {
        return;
    }

    std::optional<PlayerRunSnapshot> snapshot =
        Engine::SaveSystem::Instance()->GetValue<PlayerRunSnapshot>(PlayerRunSnapshotKey);

    if (!snapshot.has_value())
    {
        return;
    }

    playerInventory->SetItems(snapshot->inventoryItems);
    knownInventoryItems = snapshot->inventoryItems;

    if (hotbar != nullptr)
    {
        hotbar->SetSavedSlots(snapshot->hotbarSlots);
    }

    if (equipment != nullptr)
    {
        equipment->SetSavedSlots(snapshot->equipmentSlots);
    }

    Engine::StatsComponent* stats =
        playerObject != nullptr ? playerObject->GetComponent<Engine::StatsComponent>() : nullptr;

    if (stats != nullptr)
    {
        stats->SetStats(snapshot->health, ClampPlayerArmor(snapshot->armor));
        stats->SetAttackPower(snapshot->attackPower);
    }

    PlayerMovementComponent* movement =
        playerObject != nullptr ? playerObject->GetComponent<PlayerMovementComponent>() : nullptr;

    if (movement != nullptr)
    {
        movement->SetSpeed(snapshot->movementSpeed);
    }
}

void GameUIComponent::SavePlayerRunState()
{
    if (playerInventory == nullptr)
    {
        return;
    }

    PlayerRunSnapshot snapshot;
    snapshot.inventoryItems = playerInventory->GetItems();

    if (hotbar != nullptr)
    {
        snapshot.hotbarSlots = hotbar->GetSavedSlots();
    }

    if (equipment != nullptr)
    {
        snapshot.equipmentSlots = equipment->GetSavedSlots();
    }

    Engine::StatsComponent* stats =
        playerObject != nullptr ? playerObject->GetComponent<Engine::StatsComponent>() : nullptr;

    if (stats != nullptr)
    {
        snapshot.health = stats->GetHealth();
        snapshot.armor = ClampPlayerArmor(stats->GetArmor());
        snapshot.attackPower = stats->GetAttackPower();
    }

    PlayerMovementComponent* movement =
        playerObject != nullptr ? playerObject->GetComponent<PlayerMovementComponent>() : nullptr;

    if (movement != nullptr)
    {
        snapshot.movementSpeed = movement->GetSpeed();
    }

    Engine::SaveSystem::Instance()->SetValue(PlayerRunSnapshotKey, snapshot);
}

void GameUIComponent::ToggleInventory()
{
    if (inventory == nullptr)
    {
        return;
    }

    inventory->Toggle();

    // Equipment is a companion panel, so it follows inventory visibility.
    if (equipment != nullptr)
    {
        equipment->SetOpen(inventory->IsOpen());
    }

    if (!inventory->IsOpen())
    {
        ClearSelectedItem();
    }
}

void GameUIComponent::CloseInventory()
{
    if (inventory != nullptr && inventory->IsOpen())
    {
        inventory->Toggle();
    }

    if (equipment != nullptr)
    {
        equipment->SetOpen(false);
    }

    ClearSelectedItem();
}

void GameUIComponent::ClearSelectedItem()
{
    draggedItem.reset();

    if (inventory != nullptr)
    {
        inventory->ClearSelection();
    }

    if (descriptionPanel != nullptr)
    {
        descriptionPanel->Hide();
    }

    if (equipment != nullptr)
    {
        equipment->ClearHighlightedItem();
    }

    if (hotbar != nullptr)
    {
        hotbar->ClearHighlightedItem();
    }
}

void GameUIComponent::SelectItem(const UIItemView& item)
{
    draggedItem = item;

    if (descriptionPanel != nullptr)
    {
        descriptionPanel->ShowItem(item);
    }

    if (equipment != nullptr)
    {
        equipment->SetHighlightedItem(draggedItem);
    }

    if (hotbar != nullptr)
    {
        hotbar->SetHighlightedItem(draggedItem);
    }
}

bool GameUIComponent::TryPlaceSelectedItem(sf::Vector2f mousePosition)
{
    if (!draggedItem.has_value())
    {
        return false;
    }

    return TryEquipSelectedItem(mousePosition) || TryAssignSelectedItemToHotbar(mousePosition);
}

bool GameUIComponent::TryEquipSelectedItem(sf::Vector2f mousePosition)
{
    if (!draggedItem.has_value() || equipment == nullptr ||
        !equipment->ContainsPoint(mousePosition))
    {
        return false;
    }

    EquipmentPlacementPreview preview =
        equipment->PreviewPlacement(mousePosition, draggedItem.value());

    if (!preview.handled)
    {
        return false;
    }

    if (!preview.canPlace || draggedItem->stack.data == nullptr)
    {
        ShowPopupMessage("Wrong equipment slot");
        return true;
    }

    ItemStack equippedItem{draggedItem->stack.data, 1};

    if (playerInventory == nullptr || !playerInventory->RemoveOneItem(equippedItem.data))
    {
        ShowPopupMessage("Item missing");
        return true;
    }

    if (preview.replacedItem.has_value() && !playerInventory->AddItem(preview.replacedItem.value()))
    {
        playerInventory->AddItem(equippedItem);
        ShowPopupMessage("Inventory is full");
        return true;
    }

    equipment->CommitPlacement(preview, draggedItem.value());
    ApplyEquipmentChange(equippedItem.data,
                         preview.replacedItem.has_value() ? preview.replacedItem->data : nullptr);
    ShowPopupMessage("Equipped: " + equippedItem.GetName());
    return true;
}

bool GameUIComponent::TryAssignSelectedItemToHotbar(sf::Vector2f mousePosition)
{
    if (!draggedItem.has_value() || hotbar == nullptr || !hotbar->ContainsPoint(mousePosition))
    {
        return false;
    }

    if (hotbar->TryPlaceItem(mousePosition, draggedItem.value()))
    {
        ShowPopupMessage("Added to hotbar: " + draggedItem->stack.GetName());
        return true;
    }

    ShowPopupMessage("Only potions fit the hotbar");
    return true;
}

void GameUIComponent::ShowPopupMessage(const std::string& message, float duration)
{
    if (popup != nullptr)
    {
        popup->ShowMessage(message, duration);
    }
}

void GameUIComponent::DrawDraggedItem(sf::RenderWindow& window)
{
    if (!draggedItem.has_value())
    {
        return;
    }

    sf::Vector2i pixelMousePosition = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosition =
        window.mapPixelToCoords(pixelMousePosition, window.getDefaultView());

    const std::string textureKey = GetItemTextureKey(draggedItem->stack);

    if (!textureKey.empty() &&
        UITextureUtils::DrawTexture(
            window, textureKey,
            {mousePosition.x - DraggedItemTextureSize * 0.5f,
             mousePosition.y - DraggedItemTextureSize * 0.5f, DraggedItemTextureSize,
             DraggedItemTextureSize},
            DraggedItemTextureAlpha))
    {
        return;
    }

    sf::RectangleShape icon;
    icon.setPosition({mousePosition.x - DraggedItemFallbackSize * 0.5f,
                      mousePosition.y - DraggedItemFallbackSize * 0.5f});
    icon.setSize({DraggedItemFallbackSize, DraggedItemFallbackSize});

    sf::Color iconColor = draggedItem->iconColor;
    iconColor.a = DraggedItemFallbackAlpha;

    icon.setFillColor(iconColor);
    icon.setOutlineColor(sf::Color(255, 255, 255, 180));
    icon.setOutlineThickness(2.0f);

    window.draw(icon);
}
}  // namespace Roguelike
