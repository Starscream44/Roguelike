#pragma once

#include <string>

namespace Engine
{
class GameObject;
}

namespace Roguelike
{
class Floor
{
   public:
    Floor(float x, float y, const std::string& textureKey = "floor_1");

   private:
    Engine::GameObject* gameObject = nullptr;
};
}  // namespace Roguelike
