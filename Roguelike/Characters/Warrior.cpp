#include "pch.h"
#include "Warrior.h"

#include "GameConfig.h"

namespace Roguelike
{
Warrior::Warrior(Engine::GameObject* player, float x, float y)
{
    BuildEnemy(player, "Warrior", "enemy", x, y, GameConfig::WarriorHealth,
               GameConfig::WarriorArmor, GameConfig::WarriorAttackPower, GameConfig::WarriorSpeed,
               GameConfig::WarriorDetectionRadius);
}
}  // namespace Roguelike