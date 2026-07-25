// @file Trigger.h

#pragma once

namespace Engine
{
class ColliderComponent;
class GameObject;

struct Trigger
{
   public:
    Trigger(ColliderComponent* newFirst, ColliderComponent* newSecond);

    ColliderComponent* GetFirst() const;
    ColliderComponent* GetSecond() const;

    bool HasGameObject(GameObject* gameObject) const;

   private:
    ColliderComponent* first = nullptr;
    ColliderComponent* second = nullptr;
};
}  // namespace Engine