#include "GlassBlock.h"

namespace Arkanoid
{
	void GlassBlock::Init(const sf::Vector2f& position, const sf::Vector2f& size)
	{
		Block::Init(position, size);

		shape.setFillColor(sf::Color(120, 220, 255, 45));
		shape.setOutlineColor(sf::Color(170, 240, 255, 180));
		shape.setOutlineThickness(2.f);
	}

	bool GlassBlock::ShouldBounceBall() const
	{
		return false;
	}

	int GlassBlock::GetScorePoints() const
	{
		return 20;
	}

	void GlassBlock::OnHit(Collidable& collidable)
	{
		Destroy();
	}
}
