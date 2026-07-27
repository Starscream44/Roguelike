#pragma once

#include "ItemData.h"

#include <array>
#include <optional>
#include <vector>

namespace Roguelike
{
inline constexpr const char* PlayerRunSnapshotKey = "roguelike.player_run";

struct PlayerRunSnapshot
{
    std::vector<ItemStack> inventoryItems;
    std::array<std::optional<ItemStack>, 6> hotbarSlots;
    std::array<std::optional<ItemStack>, 5> equipmentSlots;

    float health = 100.0f;
    float armor = 0.0f;
    float attackPower = 25.0f;
    float movementSpeed = 500.0f;
};
}  // namespace Roguelike
