#pragma once

#include "Vector.h"

#include <vector>

namespace Roguelike
{
// Stores generated maze navigation data and builds paths through walkable tiles.
class MazeNavigation
{
   public:
    using Cell = std::pair<int, int>;

    static MazeNavigation* Instance();

    void SetMap(const std::vector<std::vector<bool>>& walkableGrid, float tileSize);

    std::vector<Engine::Vector2Df> FindPath(const Engine::Vector2Df& startWorldPosition,
                                            const Engine::Vector2Df& targetWorldPosition);

   private:
    MazeNavigation() = default;

    bool IsMapReady() const;
    bool IsInside(int x, int y) const;
    bool IsWalkable(int x, int y) const;

    // Keeps pathfinding usable when physics places an actor slightly off the grid.
    bool TryFindNearestWalkable(Cell start, Cell& result) const;

    Engine::Vector2Df CellToWorld(int x, int y) const;
    Cell WorldToCell(const Engine::Vector2Df& position) const;
    int GetCellIndex(Cell cell) const;
    int GetHeuristic(Cell from, Cell to) const;

   private:
    std::vector<std::vector<bool>> walkableGrid;

    int width = 0;
    int height = 0;

    float tileSize = 64.f;
};
}  // namespace Roguelike
