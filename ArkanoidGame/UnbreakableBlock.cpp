#include "UnbreakableBlock.h"

namespace Arkanoid
{
	void UnbreakableBlock::Init(const sf::Vector2f& position, const sf::Vector2f& size)
	{
		Block::Init(position, size);

		shape.setFillColor(sf::Color(80, 80, 80));
		shape.setOutlineColor(sf::Color::White);
		shape.setOutlineThickness(2.f);
	}

	void UnbreakableBlock::OnHit(Collidable& collidable)
	{
	}
}
