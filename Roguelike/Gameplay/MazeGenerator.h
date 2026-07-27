#pragma once

#include "Vector.h"

#include <vector>

namespace Roguelike
{
class DeveloperLevel;

class MazeGenerator
{
   public:
    MazeGenerator(int width, int height, DeveloperLevel* level);

    void Generate();

    float GetTileSize() const;

    const std::vector<Engine::Vector2Df>& GetFloorPositions() const;

   private:
    std::vector<std::pair<int, int>> GetAvailableDirections(int x, int y);

    void CarvePath(int x1, int y1, int x2, int y2);
    void BuildObjects();
    const char* GetRandomFloorTextureKey() const;
    const char* GetRandomWallTextureKey() const;

   private:
    int width = 0;
    int height = 0;

    float tileSize = 64.f;

    DeveloperLevel* level = nullptr;

    std::vector<std::vector<bool>> visited;
    std::vector<std::vector<bool>> isWall;

    std::vector<Engine::Vector2Df> floorPositions;
};
}  // namespace Roguelike
