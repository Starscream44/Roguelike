#pragma once

#include <SFML/Graphics.hpp>

#include <memory>

namespace Arkanoid
{
	class Block;

	enum class BlockType
	{
		Regular,
		ThreeHit,
		Glass,
		Unbreakable
	};

	class BlockFactory
	{
	public:
		virtual ~BlockFactory() = default;

		virtual std::unique_ptr<Block> CreateBlock(const sf::Vector2f& position, const sf::Vector2f& size) = 0;
	};

	class RegularBlockFactory final : public BlockFactory
	{
	public:
		std::unique_ptr<Block> CreateBlock(const sf::Vector2f& position, const sf::Vector2f& size) override;
	};

	class ThreeHitBlockFactory final : public BlockFactory
	{
	public:
		std::unique_ptr<Block> CreateBlock(const sf::Vector2f& position, const sf::Vector2f& size) override;
	};

	class GlassBlockFactory final : public BlockFactory
	{
	public:
		std::unique_ptr<Block> CreateBlock(const sf::Vector2f& position, const sf::Vector2f& size) override;
	};

	class UnbreakableBlockFactory final : public BlockFactory
	{
	public:
		std::unique_ptr<Block> CreateBlock(const sf::Vector2f& position, const sf::Vector2f& size) override;
	};
}
