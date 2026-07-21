#include "LevelLoader.h"

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <string>

namespace Arkanoid
{
	bool LevelLoader::LoadLevelsFromFile(const std::string& filePath)
	{
		levels.clear();

		std::ifstream file(filePath);
		if (!file.is_open())
		{
			return false;
		}

		std::string line;
		LevelData* currentLevel = nullptr;

		while (std::getline(file, line))
		{
			if (!line.empty() && line.back() == '\r')
			{
				line.pop_back();
			}

			if (line.empty())
			{
				continue;
			}

			if (line.find("Level") == 0)
			{
				levels.emplace_back();
				currentLevel = &levels.back();
				continue;
			}

			if (currentLevel == nullptr)
			{
				continue;
			}

			const int y = currentLevel->rows;
			int x = 0;

			for (char symbol : line)
			{
				if (symbol >= '0' && symbol <= '3')
				{
					currentLevel->blocks.push_back({ { x, y }, CharToBlockType(symbol) });
				}

				++x;
			}

			currentLevel->columns = std::max(currentLevel->columns, x);
			++currentLevel->rows;
		}

		return !levels.empty();
	}

	int LevelLoader::GetLevelsCount() const
	{
		return static_cast<int>(levels.size());
	}

	const LevelData& LevelLoader::GetLevel(int levelIndex) const
	{
		assert(levelIndex >= 0 && levelIndex < GetLevelsCount());
		return levels[levelIndex];
	}

	BlockType LevelLoader::CharToBlockType(char symbol) const
	{
		switch (symbol)
		{
		case '0':
			return BlockType::Regular;
		case '1':
			return BlockType::ThreeHit;
		case '2':
			return BlockType::Glass;
		case '3':
			return BlockType::Unbreakable;
		default:
			assert(false);
			return BlockType::Regular;
		}
	}
}
