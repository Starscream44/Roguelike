#include "Paddle.h"
#include <algorithm>

namespace Arkanoid
{
	void Paddle::Init(float newFieldWidth, float newFieldHeight)
	{
		fieldWidth = newFieldWidth;
		fieldHeight = newFieldHeight;

		baseWidth = 140.f;
		baseSpeed = 600.f;
		speed = baseSpeed;

		shape.setSize(sf::Vector2f(baseWidth, 20.f));
		shape.setFillColor(sf::Color::Green);

		shape.setOrigin(shape.getSize().x * 0.5f, shape.getSize().y * 0.5f);
		shape.setPosition(fieldWidth * 0.5f, fieldHeight - 40.f);
	}

	void Paddle::Update(float timeDelta)
	{
		float directionX = 0.f;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			directionX -= 1.f;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			directionX += 1.f;
		}

		shape.move(directionX * speed * timeDelta, 0.f);

		ClampInsideField();
	}

	void Paddle::Draw(sf::RenderWindow& window) const
	{
		window.draw(shape);
	}

	sf::FloatRect Paddle::GetBounds() const
	{
		return shape.getGlobalBounds();
	}

	sf::Vector2f Paddle::GetPosition() const
	{
		return shape.getPosition();
	}

	void Paddle::SetPosition(const sf::Vector2f& position)
	{
		shape.setPosition(position);
		ClampInsideField();
	}

	void Paddle::SetWidthMultiplier(float multiplier)
	{
		sf::Vector2f size = shape.getSize();
		size.x = baseWidth * multiplier;
		shape.setSize(size);
		shape.setOrigin(shape.getSize().x * 0.5f, shape.getSize().y * 0.5f);
		ClampInsideField();
	}

	void Paddle::SetSpeedMultiplier(float multiplier)
	{
		speed = baseSpeed * multiplier;
	}

	void Paddle::ResetModifiers()
	{
		SetWidthMultiplier(1.f);
		SetSpeedMultiplier(1.f);
	}

	void Paddle::OnHit(Collidable& collidable)
	{
		// Paddle does not need to react.
		// Ball reacts to the paddle.
	}

	void Paddle::ClampInsideField()
	{
		const float halfWidth = shape.getSize().x * 0.5f;

		sf::Vector2f position = shape.getPosition();

		position.x = std::max(halfWidth, position.x);
		position.x = std::min(fieldWidth - halfWidth, position.x);

		shape.setPosition(position);
	}
}
