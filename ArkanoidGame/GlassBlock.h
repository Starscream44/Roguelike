#pragma once

#include "Block.h"

namespace Arkanoid
{
	class GlassBlock : public Block
	{
	public:
		void Init(const sf::Vector2f& position, const sf::Vector2f& size) override;

		bool ShouldBounceBall() const override;
		int GetScorePoints() const override;

	protected:
		void OnHit(Collidable& collidable) override;
	};
}
