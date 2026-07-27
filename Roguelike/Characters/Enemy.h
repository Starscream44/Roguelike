#pragma once

#include "Character.h"

#include <string>

namespace Engine
{
class GameObject;
}

namespace Roguelike
{
class Enemy : public Character
{
   protected:
    void BuildEnemy(Engine::GameObject* player, const std::string& name,
                    const std::string& textureKey, float x, float y, float health, float armor,
                    float attackPower, float speed, float detectionRadius);
};
}  // namespace Roguelike