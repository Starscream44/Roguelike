#pragma once
#include "Collidable.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

namespace Arkanoid
{
	class Paddle;

	class Ball : public Collidable
	{
	public:
		void Init(float fieldWidth, float fieldHeight);
		void Restart();
		void Update(float timeDelta);
		void Draw(sf::RenderWindow& window) const;

		sf::FloatRect GetBounds() const override;
		sf::Vector2f GetPosition() const;
		sf::Vector2f GetVelocity() const;
		void SetPosition(const sf::Vector2f& position);
		void SetVelocity(const sf::Vector2f& newVelocity);
		void SetSpeedMultiplier(float multiplier);
		void ResetModifiers();
		bool IsBelowField() const;

		void BounceFromBlock(const sf::FloatRect& blockBounds);

	protected:
		void OnHit(Collidable& collidable) override;

	private:
		void ClampAndBounceFromWalls();
		void BounceFromPaddle(const Paddle& paddle);
		void PlayBounceSound();

	private:
		sf::CircleShape shape;
		sf::SoundBuffer bounceSoundBuffer;
		sf::Sound bounceSound;
		sf::Vector2f velocity = { 250.f, -250.f };
		float speedMultiplier = 1.f;

		float fieldWidth = 0.f;
		float fieldHeight = 0.f;
	};
}
