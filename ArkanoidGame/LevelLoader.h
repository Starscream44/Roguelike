#pragma once

#include "BlockFactory.h"

#include <SFML/System/Vector2.hpp>

#include <string>
#include <vector>

namespace Arkanoid
{
	struct LevelBlockData
	{
		sf::Vector2i gridPosition;
		BlockType blockType = BlockType::Regular;
	};

	struct LevelData
	{
		std::vector<LevelBlockData> blocks;
		int columns = 0;
		int rows = 0;
	};

	class LevelLoader
	{
	public:
		bool LoadLevelsFromFile(const std::string& filePath);

		int GetLevelsCount() const;
		const LevelData& GetLevel(int levelIndex) const;

	private:
		BlockType CharToBlockType(char symbol) const;

	private:
		std::vector<LevelData> levels;
	};
}
