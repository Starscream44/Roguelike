#pragma once

#include "Block.h"

#include <vector>

namespace Arkanoid
{
	class DurableBlock : public Block
	{
	public:
		void Init(
			const sf::Vector2f& position,
			const sf::Vector2f& size,
			int hitPoints,
			const std::vector<sf::Color>& damageColors
		);
		int GetScorePoints() const override;

	protected:
		void OnHit(Collidable& collidable) override;

	private:
		void UpdateColor();

	private:
		int maxHitPoints = 3;
		int currentHitPoints = 3;

		std::vector<sf::Color> damageColors;
	};
}
