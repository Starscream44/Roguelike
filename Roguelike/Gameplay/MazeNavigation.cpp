#include "pch.h"
#include "MazeNavigation.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>

namespace Roguelike
{
MazeNavigation* MazeNavigation::Instance()
{
    static MazeNavigation navigation;
    return &navigation;
}

void MazeNavigation::SetMap(const std::vector<std::vector<bool>>& newWalkableGrid,
                            float newTileSize)
{
    // MazeGenerator owns generation; MazeNavigation keeps only a lightweight walkable map.
    walkableGrid = newWalkableGrid;
    tileSize = newTileSize;

    height = static_cast<int>(walkableGrid.size());
    width = height > 0 ? static_cast<int>(walkableGrid[0].size()) : 0;
}

std::vector<Engine::Vector2Df> MazeNavigation::FindPath(
    const Engine::Vector2Df& startWorldPosition, const Engine::Vector2Df& targetWorldPosition)
{
    std::vector<Engine::Vector2Df> result;

    if (!IsMapReady())
    {
        return result;
    }

    auto start = WorldToCell(startWorldPosition);
    auto target = WorldToCell(targetWorldPosition);

    // Characters can stand slightly off-grid after collisions; snap to nearest walkable cell.
    if (!TryFindNearestWalkable(start, start) || !TryFindNearestWalkable(target, target))
    {
        return result;
    }

    if (start == target)
    {
        return result;
    }

    struct PathNode
    {
        Cell cell;
        int priority = 0;
    };

    auto compareNodes = [](const PathNode& left, const PathNode& right)
    {
        return left.priority > right.priority;
    };

    std::priority_queue<PathNode, std::vector<PathNode>, decltype(compareNodes)> openCells(
        compareNodes);
    std::vector<int> cost(width * height, std::numeric_limits<int>::max());
    std::vector<Cell> parent(width * height, {-1, -1});

    openCells.push({start, 0});
    cost[GetCellIndex(start)] = 0;

    // Four-direction movement matches the maze corridors and avoids diagonal wall clipping.
    const std::vector<Cell> directions = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    while (!openCells.empty())
    {
        Cell current = openCells.top().cell;
        openCells.pop();

        if (current == target)
        {
            break;
        }

        for (const auto& direction : directions)
        {
            int nextX = current.first + direction.first;
            int nextY = current.second + direction.second;
            Cell next = {nextX, nextY};

            if (!IsWalkable(nextX, nextY))
            {
                continue;
            }

            const int currentIndex = GetCellIndex(current);
            const int nextIndex = GetCellIndex(next);
            const int nextCost = cost[currentIndex] + 1;

            if (nextCost >= cost[nextIndex])
            {
                continue;
            }

            cost[nextIndex] = nextCost;
            parent[nextIndex] = current;

            // A*: real cost so far plus Manhattan estimate to the target.
            openCells.push({next, nextCost + GetHeuristic(next, target)});
        }
    }

    if (cost[GetCellIndex(target)] == std::numeric_limits<int>::max())
    {
        return result;
    }

    std::vector<Cell> cells;
    Cell current = target;

    while (current != start)
    {
        cells.push_back(current);
        current = parent[GetCellIndex(current)];
    }

    std::reverse(cells.begin(), cells.end());

    for (const auto& cell : cells)
    {
        result.push_back(CellToWorld(cell.first, cell.second));
    }

    return result;
}

bool MazeNavigation::IsMapReady() const
{
    if (width <= 0 || height <= 0 || static_cast<int>(walkableGrid.size()) != height)
    {
        return false;
    }

    for (const auto& row : walkableGrid)
    {
        if (static_cast<int>(row.size()) != width)
        {
            return false;
        }
    }

    return true;
}

bool MazeNavigation::IsInside(int x, int y) const
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool MazeNavigation::IsWalkable(int x, int y) const { return IsInside(x, y) && walkableGrid[y][x]; }

bool MazeNavigation::TryFindNearestWalkable(Cell start, Cell& result) const
{
    if (!IsMapReady())
    {
        return false;
    }

    start.first = std::clamp(start.first, 0, width - 1);
    start.second = std::clamp(start.second, 0, height - 1);

    if (IsWalkable(start.first, start.second))
    {
        result = start;
        return true;
    }

    std::queue<Cell> cells;
    std::vector<bool> visited(width * height, false);
    const std::vector<Cell> directions = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    cells.push(start);
    visited[GetCellIndex(start)] = true;

    // Breadth-first search returns the closest valid cell around the blocked position.
    while (!cells.empty())
    {
        Cell current = cells.front();
        cells.pop();

        for (const Cell& direction : directions)
        {
            Cell next = {current.first + direction.first, current.second + direction.second};

            if (!IsInside(next.first, next.second))
            {
                continue;
            }

            const int nextIndex = GetCellIndex(next);

            if (visited[nextIndex])
            {
                continue;
            }

            if (IsWalkable(next.first, next.second))
            {
                result = next;
                return true;
            }

            visited[nextIndex] = true;
            cells.push(next);
        }
    }

    return false;
}

Engine::Vector2Df MazeNavigation::CellToWorld(int x, int y) const
{
    // World positions use tile centers in this project, matching MazeGenerator object placement.
    return {x * tileSize, y * tileSize};
}

MazeNavigation::Cell MazeNavigation::WorldToCell(const Engine::Vector2Df& position) const
{
    // Round instead of floor so slightly displaced physics objects snap to the nearest tile.
    return {static_cast<int>(std::round(position.x / tileSize)),
            static_cast<int>(std::round(position.y / tileSize))};
}

int MazeNavigation::GetCellIndex(Cell cell) const
{
    return cell.second * width + cell.first;
}

int MazeNavigation::GetHeuristic(Cell from, Cell to) const
{
    return std::abs(from.first - to.first) + std::abs(from.second - to.second);
}
}  // namespace Roguelike
