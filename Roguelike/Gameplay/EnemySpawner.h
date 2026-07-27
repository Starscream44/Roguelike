#pragma once

#include "Vector.h"

#include <memory>
#include <vector>

namespace Engine
{
class GameObject;
}

namespace Roguelike
{
class Character;

enum class EnemyType
{
    Creeper,
    Warrior
};

struct EnemySpawnSettings
{
    int count = 5;
    float minDistanceFromPlayer = 300.f;
    EnemyType enemyType = EnemyType::Creeper;
};

// Creates enemies on generated floor tiles.
// The spawner avoids player spawn area and prevents enemies from spawning
// on the same tile.
class EnemySpawner
{
   public:
    std::vector<std::unique_ptr<Character>> Spawn(
        const EnemySpawnSettings& settings, const std::vector<Engine::Vector2Df>& floorPositions,
        Engine::GameObject* player);

   private:
    bool IsPositionFarEnoughFromPlayer(const Engine::Vector2Df& position,
                                       Engine::GameObject* player, float minDistance) const;

    bool IsPositionReachableFromPlayer(const Engine::Vector2Df& position,
                                       Engine::GameObject* player) const;

    bool IsPositionAlreadyUsed(const Engine::Vector2Df& position,
                               const std::vector<Engine::Vector2Df>& usedPositions) const;

    std::unique_ptr<Character> CreateEnemy(EnemyType enemyType, Engine::GameObject* player,
                                           const Engine::Vector2Df& position) const;
};
}  // namespace Roguelike
