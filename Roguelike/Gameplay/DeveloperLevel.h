#pragma once

#include "Character.h"
#include "Floor.h"
#include "Player.h"
#include "Scene.h"
#include "Vector.h"
#include "Wall.h"

#include <memory>
#include <vector>

namespace Engine
{
class GameObject;
}

namespace Roguelike
{
class DeveloperLevel : public Engine::Scene
{
   public:
    void Start();
    void Restart();
    void Stop();

    std::vector<std::unique_ptr<Floor>> floors;
    std::vector<std::unique_ptr<Wall>> walls;

   private:
    void LoadResources();
    void GenerateMaze();
    void CreatePlayer();
    void SpawnEnemies();
    void SpawnItems();
    void RegisterPlayerTargets();
    void RegisterProjectileObstacles();
    void CreateMusic();
    const char* ChooseBackgroundTrack() const;
    void CreateUI();

   private:
    std::shared_ptr<Player> player = nullptr;
    Engine::GameObject* playerObject = nullptr;

    std::vector<std::unique_ptr<Character>> enemies;
    std::vector<Engine::Vector2Df> floorPositions;
};
}  // namespace Roguelike
