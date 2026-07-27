#pragma once

#include "ItemData.h"

#include <array>

namespace Roguelike
{
namespace GameConfig
{
struct BackgroundTrackConfig
{
    const char* name;
    const char* path;
};

struct TextureConfig
{
    const char* key;
    const char* path;
    bool smooth = true;
};

struct CharacterTextureSet
{
    const char* baseKey;
    const char* basePath;
    const char* defaultKey;
    const char* defaultPath;
    const char* downKey;
    const char* downPath;
    const char* leftKey;
    const char* leftPath;
    const char* rightKey;
    const char* rightPath;
    const char* upKey;
    const char* upPath;
};

// Audio balance and playlist. Loader skips tracks that fail to load.
inline constexpr float MusicVolume = 25.f;
inline constexpr std::array<BackgroundTrackConfig, 4> BackgroundTracks = {
    {{"bg_level_1", "Resources/Sounds/background_1.ogg"},
     {"bg_level_2", "Resources/Sounds/background_2.ogg"},
     {"bg_level_3", "Resources/Sounds/background_3.ogg"},
     {"bg_level_4", "Resources/Sounds/background_4.ogg"}}};

inline constexpr const char* MainUIFontPath = "Resources/Fonts/MPLUSRounded1c-Regular.ttf";
inline constexpr const char* TitleFontPath = "Resources/Fonts/CGXYZPCAlt-Regular.otf";

// Fixed design resolution used by camera and screen-space UI.
inline constexpr unsigned int WindowWidth = 1920;
inline constexpr unsigned int WindowHeight = 1080;
inline constexpr float WindowCenterX = WindowWidth / 2.0f;
inline constexpr float WindowCenterY = WindowHeight / 2.0f;

// Maze grid and sprite scale. Tile positions use tile centers in world space.
inline constexpr float TileSize = 64.f;
inline constexpr int MazeWidth = 21;
inline constexpr int MazeHeight = 15;
inline constexpr int CharacterPixelSize = 48;
inline constexpr int TilePixelSize = 64;

// Level generation picks from these keys, so adding tile variants stays data-only.
inline constexpr std::array<const char*, 8> FloorTextureKeys = {
    {"floor_1", "floor_2", "floor_3", "floor_4", "floor_5", "floor_6", "floor_7", "floor_8"}};
inline constexpr std::array<const char*, 8> WallTextureKeys = {
    {"wall_1", "wall_2", "wall_3", "wall_4", "wall_5", "wall_6", "wall_7", "wall_8"}};

inline constexpr std::array<CharacterTextureSet, 3> CharacterTextures = {
    {{"player", "Resources/Textures/Player.png",
      "player_default", "Resources/Textures/Player_default.png",
      "player_down", "Resources/Textures/Player_down.png",
      "player_left", "Resources/Textures/Player_left.png",
      "player_right", "Resources/Textures/Player_right.png",
      "player_up", "Resources/Textures/Player_top.png"},
     {"creeper", "Resources/Textures/Creeper.png",
      "creeper_default", "Resources/Textures/Creeper_default.png",
      "creeper_down", "Resources/Textures/Creeper_default.png",
      "creeper_left", "Resources/Textures/Creeper_left.png",
      "creeper_right", "Resources/Textures/Creeper_right.png",
      "creeper_up", "Resources/Textures/Creeper_top.png"},
     {"enemy", "Resources/Textures/Enemy.png",
      "warrior_default", "Resources/Textures/Warrior_default.png",
      "warrior_down", "Resources/Textures/Warrior_default.png",
      "warrior_left", "Resources/Textures/Warrior_left.png",
      "warrior_right", "Resources/Textures/Warrior_right.png",
      "warrior_up", "Resources/Textures/Warrior_top.png"}}};

inline constexpr std::array<TextureConfig, 24> UITextures = {
    {{"player_projectile", "Resources/Textures/player_projectile.png", false},
     {"ui_quest_list", "Resources/Textures/quest_list.png", false},
     {"ui_inventory_menu_top", "Resources/UI/inventory_menu_top.png", false},
     {"ui_inventory_menu_grid", "Resources/UI/inventory_menu_grid.png", false},
     {"ui_inventory_menu_description", "Resources/UI/inventory_menu_description.png", false},
     {"ui_slot_equipment", "Resources/UI/slot_equipment.png", false},
     {"ui_slot_weapon", "Resources/UI/slot_weapon.png", false},
     {"ui_slot_inventory", "Resources/UI/slot_inventory.png", false},
     {"ui_player_avatar", "Resources/UI/player_avatar.png", false},
     {"ui_hud_health_bar_frame", "Resources/UI/hud_health_bar_frame.png", false},
     {"ui_hud_health_bar_fill", "Resources/UI/hud_health_bar_fill.png", false},
     {"ui_player_armor", "Resources/UI/player_armor.png", false},
     {"ui_popup_message", "Resources/UI/popup_message.png", false},
     {"ui_hotbar_frame", "Resources/UI/hotbar_frame.png", false},
     {"ui_slot_hotbar", "Resources/UI/slot_hotbar.png", false},
     {"ui_item_armor", "Resources/UI/item_armor.png", false},
     {"ui_item_helmet", "Resources/UI/item_helmet.png", false},
     {"ui_item_boots", "Resources/UI/item_boots.png", false},
     {"ui_item_speed_potion", "Resources/UI/item_speed_potion.png", false},
     {"ui_item_health_potion", "Resources/UI/item_health_potion.png", false},
     {"ui_item_attack_potion", "Resources/UI/item_attack_potion.png", false},
     {"ui_start_game_background", "Resources/UI/start_game_background.png", false},
     {"ui_start_game_fire_light", "Resources/UI/fire_start_game_background.png", false},
     {"ui_dead_background", "Resources/UI/2dead_background.png", false}}};

inline constexpr TextureConfig DeadLightBackgroundTexture = {
    "ui_dead_light_background", "Resources/UI/1dead_background.png", false};
inline constexpr TextureConfig NextLevelBackgroundTexture = {
    "ui_next_level_background", "Resources/UI/next_level_background.png", false};

// Player stats and initial maze cell.
inline constexpr float PlayerHealth = 100.f;
inline constexpr float PlayerArmor = 4.f;
inline constexpr float MaxPlayerArmor = 7.f;
inline constexpr float PlayerAttackPower = 25.f;
inline constexpr float PlayerMoveSpeed = 500.f;
inline constexpr float MaxPlayerMoveSpeed = 740.f;
inline constexpr float HealthPotionRestore = 35.f;
inline constexpr float AttackPotionBonus = 10.f;
inline constexpr float SpeedPotionBonus = 60.f;
inline constexpr float PlayerStartTileX = 1.f;
inline constexpr float PlayerStartTileY = 1.f;

// Player projectile tuning. Texture settings affect visuals only; radius is gameplay hit size.
inline constexpr float ProjectileSpeed = 700.f;
inline constexpr float ProjectileLifeTime = 1.5f;
inline constexpr float ProjectileRadius = 4.f;
inline constexpr const char* PlayerProjectileTextureKey = "player_projectile";
inline constexpr float ProjectileTextureWidth = 64.f;
inline constexpr float ProjectileTextureHeight = 34.f;

// Enemy stats. Detection radius controls when enemies start pathfinding toward the player.
inline constexpr float CreeperHealth = 50.f;
inline constexpr float CreeperArmor = 0.f;
inline constexpr float CreeperAttackPower = 30.f;
inline constexpr float CreeperSpeed = 180.f;
inline constexpr float CreeperDetectionRadius = 250.f;

inline constexpr float WarriorHealth = 100.f;
inline constexpr float WarriorArmor = 0.f;
inline constexpr float WarriorAttackPower = 15.f;
inline constexpr float WarriorSpeed = 130.f;
inline constexpr float WarriorDetectionRadius = 250.f;

// Spawn counts are baseline values; higher levels add extra enemies.
inline constexpr int CreeperSpawnCount = 1;
inline constexpr int WarriorSpawnCount = 4;
inline constexpr int SpawnMaxAttemptsMultiplier = 40;
inline constexpr float EnemyMinSpawnDistanceFromPlayer = 300.f;

// Items and inventory layout.
inline constexpr int ItemSpawnCount = 18;
inline constexpr float ItemMinSpawnDistanceFromPlayer = 180.f;
inline constexpr float ItemPickupRadius = 28.f;
inline constexpr float ItemIconSize = 30.f;
inline constexpr int InventoryColumns = 6;
inline constexpr int InventoryRows = 2;
inline constexpr int InventoryPages = 4;
inline constexpr int InventorySlotCount = InventoryColumns * InventoryRows * InventoryPages;

// Main-menu campfire animation. Position is screen-space and easy to nudge for art alignment.
inline constexpr float StartMenuFireFrameSeconds = 0.15f;
inline constexpr int StartMenuFireFrameCount = 6;
inline constexpr float StartMenuFireLeft = 806.0f;
inline constexpr float StartMenuFireTop = 638.0f;
inline constexpr float StartMenuFireWidth = 256.0f;
inline constexpr float StartMenuFireHeight = 234.0f;

// Data table for generated item pickups.
inline constexpr std::array<ItemData, 6> Items = {
    {{"Armor", "A worn breastplate. Heavy, honest protection.", ItemTag::Equipment,
      EquipmentSlotType::Armor, 1, 135, 150, 170, "ui_item_armor", ItemEffectType::None, 0.0f,
      2.0f},
     {"Helmet", "A dented helmet that still knows its job.", ItemTag::Equipment,
      EquipmentSlotType::Head, 1, 185, 180, 150, "ui_item_helmet", ItemEffectType::None, 0.0f,
      1.0f},
     {"Speed Potion", "A bright draught that makes every step lighter.", ItemTag::Consumable,
      EquipmentSlotType::None, 1, 80, 190, 235, "ui_item_speed_potion",
      ItemEffectType::IncreaseSpeed, SpeedPotionBonus},
     {"Health Potion", "A warm red potion with a reassuring glow.", ItemTag::Consumable,
      EquipmentSlotType::None, 1, 205, 55, 75, "ui_item_health_potion",
      ItemEffectType::RestoreHealth, HealthPotionRestore},
     {"Attack Potion", "A sharp bitter brew that wakes up your weapon hand.", ItemTag::Consumable,
      EquipmentSlotType::None, 1, 220, 105, 50, "ui_item_attack_potion",
      ItemEffectType::IncreaseAttack, AttackPotionBonus},
     {"Boots", "Soft leather boots made for quick exits.", ItemTag::Equipment,
      EquipmentSlotType::Boots, 1, 150, 95, 55, "ui_item_boots", ItemEffectType::None, 0.0f,
      0.0f, 0.0f, 40.0f}}};

}  // namespace GameConfig
}  // namespace Roguelike
