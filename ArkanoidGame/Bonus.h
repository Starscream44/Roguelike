#pragma once

#include "Ball.h"
#include "Paddle.h"

#include <SFML/Graphics.hpp>

#include <memory>
#include <random>
#include <string>

namespace Arkanoid
{
	class BonusEffect
	{
	public:
		virtual ~BonusEffect() = default;

		virtual void Apply(Paddle& paddle, Ball& ball, int& health) = 0;
		virtual void Revert(Paddle& paddle, Ball& ball, int& health) = 0;
		virtual float GetDuration() const = 0;
		virtual sf::Color GetColor() const = 0;
		virtual std::string GetName() const = 0;
	};

	class WidePaddleEffect final : public BonusEffect
	{
	public:
		void Apply(Paddle& paddle, Ball& ball, int& health) override;
		void Revert(Paddle& paddle, Ball& ball, int& health) override;
		float GetDuration() const override;
		sf::Color GetColor() const override;
		std::string GetName() const override;
	};

	class ExtraLifeEffect final : public BonusEffect
	{
	public:
		void Apply(Paddle& paddle, Ball& ball, int& health) override;
		void Revert(Paddle& paddle, Ball& ball, int& health) override;
		float GetDuration() const override;
		sf::Color GetColor() const override;
		std::string GetName() const override;
	};

	class FastPaddleEffect final : public BonusEffect
	{
	public:
		void Apply(Paddle& paddle, Ball& ball, int& health) override;
		void Revert(Paddle& paddle, Ball& ball, int& health) override;
		float GetDuration() const override;
		sf::Color GetColor() const override;
		std::string GetName() const override;
	};

	class Bonus
	{
	public:
		void Init(const sf::Vector2f& position, std::unique_ptr<BonusEffect> effect);
		void Update(float timeDelta);
		void Draw(sf::RenderWindow& window) const;

		bool IsMissed(float fieldHeight) const;
		bool IsCollected(const Paddle& paddle) const;
		std::unique_ptr<BonusEffect> TakeEffect();

	private:
		sf::RectangleShape shape;
		std::unique_ptr<BonusEffect> effect;
		float fallSpeed = 160.f;
	};

	class BonusFactory
	{
	public:
		BonusFactory();

		bool ShouldSpawnBonus();
		std::unique_ptr<Bonus> CreateRandomBonus(const sf::Vector2f& position);

	private:
		std::unique_ptr<BonusEffect> CreateRandomEffect();

	private:
		std::mt19937 generator;
	};
}
