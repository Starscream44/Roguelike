#pragma once

#include "CameraComponent.h"
#include "Character.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "InputComponent.h"
#include "RenderSystem.h"
#include "SpriteRendererComponent.h"

#include <vector>

namespace Roguelike
{
class Player : public Character
{
   public:
    Player();

    void AddAttackTarget(Engine::GameObject* target);
    void SetObstacles(const std::vector<Engine::GameObject*>& obstacles);
};
}  // namespace Roguelike