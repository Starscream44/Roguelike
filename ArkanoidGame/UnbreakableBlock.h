#pragma once

#include "Block.h"

namespace Arkanoid
{
	class UnbreakableBlock : public Block
	{
	public:
		void Init(const sf::Vector2f& position, const sf::Vector2f& size) override;

	protected:
		void OnHit(Collidable& collidable) override;
	};
}
