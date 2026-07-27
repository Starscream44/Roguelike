#pragma once

#include "Component.h"
#include "EquipmentPanel.h"
#include "GameObject.h"
#include "GameScreenOverlay.h"
#include "HUD.h"
#include "HotbarPanel.h"
#include "InventoryComponent.h"
#include "InventoryPanel.h"
#include "ItemDescriptionPanel.h"
#include "PopupMessage.h"
#include "UIItemView.h"
#include "UIManager.h"

#include <SFML/Graphics.hpp>

#include <optional>
#include <string>
#include <vector>

namespace Roguelike
{
class GameUIComponent : public Engine::Component
{
   public:
    explicit GameUIComponent(Engine::GameObject* gameObject);

    void SetPlayer(Engine::GameObject* player);
    void SetLevelObjective(const std::vector<Engine::GameObject*>& enemies, int level);

    void HandleEvent(const sf::Event& event) override;
    void Update(float deltaTime) override;
    void Render() override;

   private:
    // UI creation and frame refresh.
    void CreateUI();
    void UpdateHUD();
    void UpdateHUDStats(float health, float armor);
    void UpdateInventory();
    void UpdateLevelObjective(float deltaTime);

    // Inventory/hotbar synchronization.
    void AutoPlaceNewHotbarItems(const std::vector<ItemStack>& items);
    int GetKnownItemCount(const ItemData* itemData) const;

    // Modal input flow. Game over, level complete, main menu, and pause consume input first.
    void HandleKeyPressed(sf::Keyboard::Key key);
    void HandleMouseButtonPressed(const sf::Event::MouseButtonEvent& mouseButton,
                                  sf::RenderWindow& window);
    void HandleStartPressed();
    void HandlePausePressed();
    void HandleInventoryTogglePressed();
    void HandleInventoryClick(sf::Vector2f mousePosition);
    void HandleInventoryCancelPressed();
    void HandleHotbarKey(sf::Keyboard::Key key);
    void HandleDeathState();
    void HandleGameOverRestartPressed();
    void HandleLevelCompleteNextPressed();

    // Consumable effects and run persistence between levels.
    std::string ApplyHotbarItemEffect(const ItemData& itemData);
    void ApplyEquipmentChange(const ItemData* equippedItem, const ItemData* replacedItem);
    void ApplyEquipmentBonuses(const ItemData* itemData, float direction);
    void RestorePlayerRunState();
    void SavePlayerRunState();

    // Drag-and-drop selection helpers shared by inventory, equipment, hotbar, and description.
    void ToggleInventory();
    void CloseInventory();
    void ClearSelectedItem();
    void SelectItem(const UIItemView& item);
    bool TryPlaceSelectedItem(sf::Vector2f mousePosition);
    bool TryEquipSelectedItem(sf::Vector2f mousePosition);
    bool TryAssignSelectedItemToHotbar(sf::Vector2f mousePosition);
    void ShowPopupMessage(const std::string& message, float duration = 1.5f);
    void DrawDraggedItem(sf::RenderWindow& window);

   private:
    Engine::GameObject* playerObject = nullptr;
    InventoryComponent* playerInventory = nullptr;

    // Objective state: enemies are tracked as alive/total, HUD converts to killed/total.
    std::vector<Engine::GameObject*> objectiveEnemies;
    int levelNumber = 1;
    int totalEnemyCount = 0;
    int aliveEnemyCount = 0;
    bool isLevelComplete = false;

    Engine::UIManager uiManager;

    // Fonts are loaded once and passed by reference into every panel.
    sf::Font font;
    sf::Font titleFont;

    HUD* hud = nullptr;
    InventoryPanel* inventory = nullptr;
    EquipmentPanel* equipment = nullptr;
    HotbarPanel* hotbar = nullptr;
    ItemDescriptionPanel* descriptionPanel = nullptr;
    PopupMessage* popup = nullptr;
    GameScreenOverlay* overlay = nullptr;

    // Currently dragged inventory item and snapshot used for hotbar auto-placement.
    std::optional<UIItemView> draggedItem;
    std::vector<ItemStack> knownInventoryItems;

    // Modal state flags. Only one top-level overlay should own input at a time.
    bool isMainMenuOpen = true;
    bool isPauseOpen = false;
    bool isGameOver = false;

};
}  // namespace Roguelike
