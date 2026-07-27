#pragma once

#include <string>

namespace Engine
{
class GameObject;
}

namespace Roguelike
{
class Wall
{
   public:
    Wall(float x, float y, const std::string& textureKey = "wall_1");

    Wall(const Wall&) = delete;
    Wall& operator=(const Wall&) = delete;

    Engine::GameObject* GetGameObject() const;

   private:
    Engine::GameObject* gameObject = nullptr;
};
}  // namespace Roguelike
