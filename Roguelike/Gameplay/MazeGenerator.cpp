#include "pch.h"
#include "MazeGenerator.h"

#include "DeveloperLevel.h"
#include "Floor.h"
#include "GameConfig.h"
#include "Logger.h"
#include "MazeNavigation.h"
#include "Wall.h"

#include <cstdlib>
#include <stack>

namespace Roguelike
{
MazeGenerator::MazeGenerator(int newWidth, int newHeight, DeveloperLevel* newLevel)
    : width(newWidth), height(newHeight), tileSize(GameConfig::TileSize), level(newLevel)
{
    if (width % 2 == 0)
    {
        width++;
    }

    if (height % 2 == 0)
    {
        height++;
    }

    // Recursive backtracking works on odd cells with wall cells between corridors.
    visited.resize(height, std::vector<bool>(width, false));
    isWall.resize(height, std::vector<bool>(width, true));
}

void MazeGenerator::Generate()
{
    LOG_INFO("Maze generation started.");

    // Start from the same inner cell as the player spawn area so the first corridor is reachable.
    int startX = 1;
    int startY = 1;

    std::stack<std::pair<int, int>> stack;
    stack.push({startX, startY});

    visited[startY][startX] = true;
    isWall[startY][startX] = false;

    while (!stack.empty())
    {
        auto [x, y] = stack.top();

        std::vector<std::pair<int, int>> directions = GetAvailableDirections(x, y);

        if (directions.empty())
        {
            stack.pop();
            continue;
        }

        std::pair<int, int> direction = directions[std::rand() % directions.size()];

        int nextX = x + direction.first;
        int nextY = y + direction.second;

        CarvePath(x, y, nextX, nextY);

        visited[nextY][nextX] = true;

        stack.push({nextX, nextY});
    }

    BuildObjects();

    LOG_INFO("Maze generation finished.");
}

float MazeGenerator::GetTileSize() const { return tileSize; }

const std::vector<Engine::Vector2Df>& MazeGenerator::GetFloorPositions() const
{
    return floorPositions;
}

std::vector<std::pair<int, int>> MazeGenerator::GetAvailableDirections(int x, int y)
{
    // Move two grid cells at a time so the cell between can be carved as a corridor.
    std::vector<std::pair<int, int>> directions = {{0, -2}, {0, 2}, {-2, 0}, {2, 0}};

    std::vector<std::pair<int, int>> available;

    for (const auto& direction : directions)
    {
        int nextX = x + direction.first;
        int nextY = y + direction.second;

        if (nextX <= 0 || nextY <= 0)
        {
            continue;
        }

        if (nextX >= width - 1 || nextY >= height - 1)
        {
            continue;
        }

        if (!visited[nextY][nextX])
        {
            available.push_back(direction);
        }
    }

    return available;
}

void MazeGenerator::CarvePath(int x1, int y1, int x2, int y2)
{
    int wallX = (x1 + x2) / 2;
    int wallY = (y1 + y2) / 2;

    // Open both target cells and the wall between them.
    isWall[y1][x1] = false;
    isWall[wallY][wallX] = false;
    isWall[y2][x2] = false;
}

void MazeGenerator::BuildObjects()
{
    if (level == nullptr)
    {
        LOG_ERROR("MazeGenerator failed. DeveloperLevel is null.");
        return;
    }

    floorPositions.clear();

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float worldX = x * tileSize;
            float worldY = y * tileSize;

            level->floors.push_back(
                std::make_unique<Floor>(worldX, worldY, GetRandomFloorTextureKey()));

            if (!isWall[y][x])
            {
                floorPositions.push_back({worldX, worldY});
            }

            if (isWall[y][x])
            {
                level->walls.push_back(
                    std::make_unique<Wall>(worldX, worldY, GetRandomWallTextureKey()));
            }
        }
    }

    // Builds a walkable grid for enemy pathfinding after maze generation.
    std::vector<std::vector<bool>> walkableGrid(height, std::vector<bool>(width, false));

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            walkableGrid[y][x] = !isWall[y][x];
        }
    }

    MazeNavigation::Instance()->SetMap(walkableGrid, tileSize);

    LOG_INFO("Maze objects created. Floors: " + std::to_string(level->floors.size()) +
             ", walls: " + std::to_string(level->walls.size()));
}

const char* MazeGenerator::GetRandomFloorTextureKey() const
{
    // Tile variants are data-driven: add loaded texture keys to GameConfig to extend visuals.
    return GameConfig::FloorTextureKeys[std::rand() % GameConfig::FloorTextureKeys.size()];
}

const char* MazeGenerator::GetRandomWallTextureKey() const
{
    // Keeping random choice here lets Floor and Wall stay simple renderable objects.
    return GameConfig::WallTextureKeys[std::rand() % GameConfig::WallTextureKeys.size()];
}
}  // namespace Roguelike
