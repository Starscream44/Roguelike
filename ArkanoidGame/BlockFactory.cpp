#include "BlockFactory.h"

#include "Block.h"
#include "DurableBlock.h"
#include "GlassBlock.h"
#include "UnbreakableBlock.h"

namespace Arkanoid
{
	std::unique_ptr<Block> RegularBlockFactory::CreateBlock(const sf::Vector2f& position, const sf::Vector2f& size)
	{
		auto block = std::make_unique<Block>();
		block->Init(position, size);
		return block;
	}

	std::unique_ptr<Block> ThreeHitBlockFactory::CreateBlock(const sf::Vector2f& position, const sf::Vector2f& size)
	{
		auto block = std::make_unique<DurableBlock>();
		block->Init(
			position,
			size,
			3,
			{
				sf::Color::Red,
				sf::Color::Yellow,
				sf::Color::Blue
			}
		);

		return block;
	}

	std::unique_ptr<Block> GlassBlockFactory::CreateBlock(const sf::Vector2f& position, const sf::Vector2f& size)
	{
		auto block = std::make_unique<GlassBlock>();
		block->Init(position, size);
		return block;
	}

	std::unique_ptr<Block> UnbreakableBlockFactory::CreateBlock(const sf::Vector2f& position, const sf::Vector2f& size)
	{
		auto block = std::make_unique<UnbreakableBlock>();
		block->Init(position, size);
		return block;
	}
}
