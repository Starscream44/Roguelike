#pragma once
#include "Collidable.h"
#include <SFML/Graphics.hpp>

namespace Arkanoid
{
	class Paddle : public Collidable
	{
	public:
		void Init(float fieldWidth, float fieldHeight);
		void Update(float timeDelta);
		void Draw(sf::RenderWindow& window) const;

		sf::FloatRect GetBounds() const override;
		sf::Vector2f GetPosition() const;
		void SetPosition(const sf::Vector2f& position);
		void SetWidthMultiplier(float multiplier);
		void SetSpeedMultiplier(float multiplier);
		void ResetModifiers();

	protected:
		void OnHit(Collidable& collidable) override;

	private:
		void ClampInsideField();

	private:
		sf::RectangleShape shape;

		float speed = 600.f;
		float baseSpeed = 600.f;
		float baseWidth = 140.f;

		float fieldWidth = 0.f;
		float fieldHeight = 0.f;
	};
}
