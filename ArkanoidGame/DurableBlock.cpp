#include "DurableBlock.h"

#include <algorithm>

namespace Arkanoid
{
	void DurableBlock::Init(
		const sf::Vector2f& position,
		const sf::Vector2f& size,
		int hitPoints,
		const std::vector<sf::Color>& damageColors
	)
	{
		Block::Init(position, size);

		maxHitPoints = std::max(1, hitPoints);
		currentHitPoints = maxHitPoints;

		this->damageColors = damageColors;

		UpdateColor();
	}

	int DurableBlock::GetScorePoints() const
	{
		return 30;
	}

	void DurableBlock::OnHit(Collidable& collidable)
	{
		if (isDestroyed)
		{
			return;
		}

		--currentHitPoints;

		if (currentHitPoints <= 0)
		{
			Destroy();
			return;
		}

		UpdateColor();
	}

	void DurableBlock::UpdateColor()
	{
		if (damageColors.empty())
		{
			return;
		}

		if (maxHitPoints <= 1)
		{
			shape.setFillColor(damageColors.back());
			return;
		}

		const int damageTaken = maxHitPoints - currentHitPoints;

		const int colorIndex = std::min(
			damageTaken,
			static_cast<int>(damageColors.size()) - 1
		);

		shape.setFillColor(damageColors[colorIndex]);
	}
}
