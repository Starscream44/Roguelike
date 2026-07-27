#include "pch.h"
#include "Creeper.h"

#include "GameConfig.h"

namespace Roguelike
{
Creeper::Creeper(Engine::GameObject* player, float x, float y)
{
    BuildEnemy(player, "Creeper", "creeper", x, y, GameConfig::CreeperHealth,
               GameConfig::CreeperArmor, GameConfig::CreeperAttackPower, GameConfig::CreeperSpeed,
               GameConfig::CreeperDetectionRadius);
}
}  // namespace Roguelike