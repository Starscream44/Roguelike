// @file GameWorld.h

#pragma once

#include "GameObject.h"
#include "PhysicsSystem.h"

#include <SFML/Window/Event.hpp>

#include <string>
#include <vector>

namespace Engine
{
class GameWorld
{
   public:
    static GameWorld* Instance();

    void HandleEvent(const sf::Event& event);
    void Update(float deltaTime);
    void FixedUpdate(float deltaTime);
    void Render();
    void LateUpdate();

    GameObject* CreateGameObject();
    GameObject* CreateGameObject(std::string name);

    void DestroyGameObject(GameObject* gameObject);
    bool IsGameObjectAlive(GameObject* gameObject) const;

    void SetPaused(bool value);
    bool IsPaused() const;

    void AddPauseIgnoredGameObject(GameObject* gameObject);
    void RemovePauseIgnoredGameObject(GameObject* gameObject);

    void Clear();
    void Print() const;

   private:
    GameWorld() {}
    ~GameWorld() {}

    GameWorld(GameWorld const&) = delete;
    GameWorld& operator=(GameWorld const&) = delete;

    bool ShouldUpdateGameObject(GameObject* gameObject) const;
    bool IsPauseIgnored(GameObject* gameObject) const;

    void DestroyGameObjectImmediate(GameObject* gameObject);

   private:
    bool isPaused = false;

    float fixedCounter = 0.0f;

    std::vector<GameObject*> gameObjects = {};
    std::vector<GameObject*> markedToDestroyGameObjects = {};
    std::vector<GameObject*> pauseIgnoredGameObjects = {};
};
}  // namespace Engine
