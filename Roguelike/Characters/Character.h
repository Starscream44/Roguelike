#pragma once

namespace Engine
{
class GameObject;
}

namespace Roguelike
{
class Character
{
   public:
    virtual ~Character() = default;

    Engine::GameObject* GetGameObject() const;

   protected:
    Engine::GameObject* gameObject = nullptr;
};
}  // namespace Roguelike