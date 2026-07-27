#pragma once

namespace Roguelike
{
class LevelBuilder
{
   public:
    LevelBuilder(float startX, float startY, float tileSize, int width, int height);

    void Build();

    float GetPlayerSpawnX() const;
    float GetPlayerSpawnY() const;

    float GetEnemySpawnX() const;
    float GetEnemySpawnY() const;

   private:
    float startX = 0.f;
    float startY = 0.f;
    float tileSize = 64.f;

    int width = 0;
    int height = 0;

    void BuildFloor();
    void BuildWalls();

    void CreateFloorTile(int x, int y);
    void CreateWallTile(int x, int y);
};
}  // namespace Roguelike