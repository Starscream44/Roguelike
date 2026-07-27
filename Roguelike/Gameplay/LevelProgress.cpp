#include "pch.h"
#include "LevelProgress.h"

namespace Roguelike
{
int LevelProgress::currentLevel = 1;

int LevelProgress::GetLevel() { return currentLevel; }

void LevelProgress::Advance() { currentLevel++; }

void LevelProgress::Reset() { currentLevel = 1; }
}  // namespace Roguelike
