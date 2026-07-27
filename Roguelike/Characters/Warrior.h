#pragma once

#include "Enemy.h"

namespace Engine
{
class GameObject;
}

namespace Roguelike
{
class Warrior : public Enemy
{
   public:
    Warrior(Engine::GameObject* player, float x, float y);

    Warrior(const Warrior&) = delete;
    Warrior& operator=(const Warrior&) = delete;
};
}  // namespace Roguelike