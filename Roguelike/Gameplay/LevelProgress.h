#pragma once

namespace Roguelike
{
class LevelProgress
{
   public:
    static int GetLevel();
    static void Advance();
    static void Reset();

   private:
    static int currentLevel;
};
}  // namespace Roguelike
