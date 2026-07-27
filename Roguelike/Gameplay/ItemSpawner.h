#pragma once

#include "Vector.h"

#include <vector>

namespace Engine
{
class GameObject;
}

namespace Roguelike
{
class ItemSpawner
{
   public:
    void Spawn(const std::vector<Engine::Vector2Df>& floorPositions, Engine::GameObject* player);

   private:
    bool IsPositionFarEnoughFromPlayer(const Engine::Vector2Df& position,
                                       Engine::GameObject* player,
                                       float minDistance) const;
};
}  // namespace Roguelike
