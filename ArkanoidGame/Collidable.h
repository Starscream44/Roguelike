#pragma once
#include <SFML/Graphics.hpp>

namespace Arkanoid
{
	class Collidable
	{
	protected:
		virtual void OnHit(Collidable& collidable) = 0;

	public:
		virtual ~Collidable() = default;

		bool CheckCollision(Collidable& collidable)
		{
			if (GetCollision(collidable) && collidable.GetCollision(*this))
			{
				OnHit(collidable);
				collidable.OnHit(*this);

				return true;
			}

			return false;
		}

		virtual bool GetCollision(const Collidable& collidable) const
		{
			return GetBounds().intersects(collidable.GetBounds());
		}

		virtual sf::FloatRect GetBounds() const = 0;
	};
}