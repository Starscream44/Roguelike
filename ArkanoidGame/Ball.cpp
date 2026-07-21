#include "Ball.h"
#include "Paddle.h"
#include "Block.h"
#include "GameSettings.h"

#include <assert.h>
#include <cmath>


namespace Arkanoid
{
	void Ball::Init(float newFieldWidth, float newFieldHeight)
	{
		fieldWidth = newFieldWidth;
		fieldHeight = newFieldHeight;

		shape.setRadius(10.f);
		shape.setFillColor(sf::Color::White);
		shape.setOrigin(shape.getRadius(), shape.getRadius());
		assert(bounceSoundBuffer.loadFromFile(SETTINGS.BALL_BOUNCE_SOUND_PATH));
		bounceSound.setBuffer(bounceSoundBuffer);
		Restart();
	}

	void Ball::Restart()
	{
		shape.setPosition(fieldWidth * 0.5f, fieldHeight * 0.5f);
		velocity = { 250.f, -250.f };
		speedMultiplier = 1.f;
	}

	void Ball::Update(float timeDelta)
	{
		shape.move(velocity * timeDelta);

		ClampAndBounceFromWalls();
	}

	void Ball::Draw(sf::RenderWindow& window) const
	{
		window.draw(shape);
	}

	sf::FloatRect Ball::GetBounds() const
	{
		return shape.getGlobalBounds();
	}

	sf::Vector2f Ball::GetPosition() const
	{
		return shape.getPosition();
	}

	sf::Vector2f Ball::GetVelocity() const
	{
		return velocity;
	}

	void Ball::SetPosition(const sf::Vector2f& position)
	{
		shape.setPosition(position);
	}

	void Ball::SetVelocity(const sf::Vector2f& newVelocity)
	{
		velocity = newVelocity;
	}

	void Ball::SetSpeedMultiplier(float multiplier)
	{
		if (speedMultiplier != 0.f)
		{
			velocity /= speedMultiplier;
		}

		speedMultiplier = multiplier;
		velocity *= speedMultiplier;
	}

	void Ball::ResetModifiers()
	{
		SetSpeedMultiplier(1.f);
	}

	bool Ball::IsBelowField() const
	{
		return GetBounds().top > fieldHeight;
	}

	void Ball::OnHit(Collidable& collidable)
	{
		if (Paddle* paddle = dynamic_cast<Paddle*>(&collidable))
		{
			BounceFromPaddle(*paddle);
		}
		else if (Block* block = dynamic_cast<Block*>(&collidable))
		{
			if (block->ShouldBounceBall())
			{
				BounceFromBlock(block->GetBounds());
			}
		}
	}

	void Ball::ClampAndBounceFromWalls()
	{
		const float radius = shape.getRadius();
		sf::Vector2f position = shape.getPosition();
		bool bounced = false;

		if (position.x - radius <= 0.f && velocity.x < 0.f)
		{
			position.x = radius;
			velocity.x = -velocity.x;
			bounced = true;
		}
		else if (position.x + radius >= fieldWidth && velocity.x > 0.f)
		{
			position.x = fieldWidth - radius;
			velocity.x = -velocity.x;
			bounced = true;
		}

		if (position.y - radius <= 0.f && velocity.y < 0.f)
		{
			position.y = radius;
			velocity.y = -velocity.y;
			bounced = true;
		}
		shape.setPosition(position);

		if (bounced)
		{
			PlayBounceSound();
		}
	}

	void Ball::BounceFromPaddle(const Paddle& paddle)
	{
		const sf::FloatRect paddleBounds = paddle.GetBounds();

		if (GetBounds().intersects(paddleBounds) && velocity.y > 0.f)
		{
			const float paddleCenterX = paddleBounds.left + paddleBounds.width * 0.5f;
			const float ballX = shape.getPosition().x;

			float hitOffset = (ballX - paddleCenterX) / (paddleBounds.width * 0.5f);

			if (hitOffset < -1.f)
			{
				hitOffset = -1.f;
			}
			else if (hitOffset > 1.f)
			{
				hitOffset = 1.f;
			}

			velocity.x = hitOffset * 300.f;
			velocity.y = -std::abs(velocity.y);

			shape.setPosition(ballX, paddleBounds.top - shape.getRadius());
			PlayBounceSound();
		}
	}

	void Ball::BounceFromBlock(const sf::FloatRect& blockBounds)
	{
		const sf::FloatRect ballBounds = GetBounds();

		if (!ballBounds.intersects(blockBounds))
		{
			return;
		}

		const float ballCenterX = ballBounds.left + ballBounds.width * 0.5f;
		const float ballCenterY = ballBounds.top + ballBounds.height * 0.5f;

		const float blockCenterX = blockBounds.left + blockBounds.width * 0.5f;
		const float blockCenterY = blockBounds.top + blockBounds.height * 0.5f;

		const float overlapX = ballBounds.width * 0.5f + blockBounds.width * 0.5f - std::abs(ballCenterX - blockCenterX);
		const float overlapY = ballBounds.height * 0.5f + blockBounds.height * 0.5f - std::abs(ballCenterY - blockCenterY);

		sf::Vector2f position = shape.getPosition();

		if (overlapX < overlapY)
		{
			velocity.x = -velocity.x;

			if (ballCenterX < blockCenterX)
			{
				position.x -= overlapX;
			}
			else
			{
				position.x += overlapX;
			}
		}
		else
		{
			velocity.y = -velocity.y;

			if (ballCenterY < blockCenterY)
			{
				position.y -= overlapY;
			}
			else
			{
				position.y += overlapY;
			}
		}

		shape.setPosition(position);
		PlayBounceSound();
	}

	void Ball::PlayBounceSound()
	{
		bounceSound.play();
	}
}
