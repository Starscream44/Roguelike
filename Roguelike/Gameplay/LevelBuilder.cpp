#include "pch.h"
#include "LevelBuilder.h"

#include "Floor.h"
#include "Wall.h"

namespace Roguelike
{
LevelBuilder::LevelBuilder(float startX, float startY, float tileSize, int width, int height)
{
    this->startX = startX;
    this->startY = startY;
    this->tileSize = tileSize;
    this->width = width;
    this->height = height;
}

void LevelBuilder::Build()
{
    BuildFloor();
    BuildWalls();
}

void LevelBuilder::BuildFloor()
{
    for (int x = 1; x < width - 1; x++)
    {
        for (int y = 1; y < height - 1; y++)
        {
            CreateFloorTile(x, y);
        }
    }
}

void LevelBuilder::BuildWalls()
{
    for (int x = 0; x < width; x++)
    {
        CreateWallTile(x, 0);
        CreateWallTile(x, height - 1);
    }

    for (int y = 1; y < height - 1; y++)
    {
        CreateWallTile(0, y);
        CreateWallTile(width - 1, y);
    }
}

void LevelBuilder::CreateFloorTile(int x, int y)
{
    new Floor(startX + x * tileSize, startY + y * tileSize);
}

void LevelBuilder::CreateWallTile(int x, int y)
{
    new Wall(startX + x * tileSize, startY + y * tileSize);
}

float LevelBuilder::GetPlayerSpawnX() const { return startX + 3 * tileSize; }

float LevelBuilder::GetPlayerSpawnY() const { return startY + 4 * tileSize; }

float LevelBuilder::GetEnemySpawnX() const { return startX + 8 * tileSize; }

float LevelBuilder::GetEnemySpawnY() const { return startY + 4 * tileSize; }
}  // namespace Roguelike