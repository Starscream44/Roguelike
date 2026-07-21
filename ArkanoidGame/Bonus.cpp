#include "Bonus.h"

namespace Arkanoid
{
	void WidePaddleEffect::Apply(Paddle& paddle, Ball& ball, int& health)
	{
		paddle.SetWidthMultiplier(1.6f);
	}

	void WidePaddleEffect::Revert(Paddle& paddle, Ball& ball, int& health)
	{
		paddle.SetWidthMultiplier(1.f);
	}

	float WidePaddleEffect::GetDuration() const
	{
		return 8.f;
	}

	sf::Color WidePaddleEffect::GetColor() const
	{
		return sf::Color(80, 200, 255);
	}

	std::string WidePaddleEffect::GetName() const
	{
		return "Wide paddle";
	}

	void ExtraLifeEffect::Apply(Paddle& paddle, Ball& ball, int& health)
	{
		++health;
	}

	void ExtraLifeEffect::Revert(Paddle& paddle, Ball& ball, int& health)
	{
	}

	float ExtraLifeEffect::GetDuration() const
	{
		return 0.f;
	}

	sf::Color ExtraLifeEffect::GetColor() const
	{
		return sf::Color(90, 255, 120);
	}

	std::string ExtraLifeEffect::GetName() const
	{
		return "Extra life";
	}

	void FastPaddleEffect::Apply(Paddle& paddle, Ball& ball, int& health)
	{
		paddle.SetSpeedMultiplier(1.5f);
	}

	void FastPaddleEffect::Revert(Paddle& paddle, Ball& ball, int& health)
	{
		paddle.SetSpeedMultiplier(1.f);
	}

	float FastPaddleEffect::GetDuration() const
	{
		return 6.f;
	}

	sf::Color FastPaddleEffect::GetColor() const
	{
		return sf::Color(255, 200, 60);
	}

	std::string FastPaddleEffect::GetName() const
	{
		return "Fast paddle";
	}

	void Bonus::Init(const sf::Vector2f& position, std::unique_ptr<BonusEffect> newEffect)
	{
		effect = std::move(newEffect);

		shape.setSize({ 24.f, 24.f });
		shape.setOrigin(shape.getSize().x * 0.5f, shape.getSize().y * 0.5f);
		shape.setPosition(position);
		shape.setFillColor(effect->GetColor());
		shape.setOutlineColor(sf::Color::White);
		shape.setOutlineThickness(2.f);
	}

	void Bonus::Update(float timeDelta)
	{
		shape.move(0.f, fallSpeed * timeDelta);
	}

	void Bonus::Draw(sf::RenderWindow& window) const
	{
		window.draw(shape);
	}

	bool Bonus::IsMissed(float fieldHeight) const
	{
		return shape.getGlobalBounds().top > fieldHeight;
	}

	bool Bonus::IsCollected(const Paddle& paddle) const
	{
		return shape.getGlobalBounds().intersects(paddle.GetBounds());
	}

	std::unique_ptr<BonusEffect> Bonus::TakeEffect()
	{
		return std::move(effect);
	}

	BonusFactory::BonusFactory()
		: generator(std::random_device{}())
	{
	}

	bool BonusFactory::ShouldSpawnBonus()
	{
		std::uniform_real_distribution<float> distribution(0.f, 1.f);
		return distribution(generator) <= 0.1f;
	}

	std::unique_ptr<Bonus> BonusFactory::CreateRandomBonus(const sf::Vector2f& position)
	{
		auto bonus = std::make_unique<Bonus>();
		bonus->Init(position, CreateRandomEffect());
		return bonus;
	}

	std::unique_ptr<BonusEffect> BonusFactory::CreateRandomEffect()
	{
		std::uniform_int_distribution<int> distribution(0, 2);
		switch (distribution(generator))
		{
		case 0:
			return std::make_unique<WidePaddleEffect>();
		case 1:
			return std::make_unique<ExtraLifeEffect>();
		default:
			return std::make_unique<FastPaddleEffect>();
		}
	}
}
