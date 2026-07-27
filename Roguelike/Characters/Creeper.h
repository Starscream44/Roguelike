#pragma once

#include "Enemy.h"

namespace Engine
{
class GameObject;
}

namespace Roguelike
{
class Creeper : public Enemy
{
   public:
    Creeper(Engine::GameObject* player, float x, float y);

    Creeper(const Creeper&) = delete;
    Creeper& operator=(const Creeper&) = delete;
};
}  // namespace Roguelike