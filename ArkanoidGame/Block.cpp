#include "Block.h"

#include "BlockObserver.h"
#include "GameSettings.h"

#include <SFML/Audio.hpp>

#include <assert.h>
#include <algorithm>

namespace Arkanoid
{
	void PlayBlockDestroySound()
	{
		static sf::SoundBuffer buffer;
		static sf::Sound sound;
		static bool isLoaded = false;

		if (!isLoaded)
		{
			assert(buffer.loadFromFile(SETTINGS.BLOCK_DESTROY_SOUND_PATH));
			sound.setBuffer(buffer);
			isLoaded = true;
		}

		sound.play();
	}

	void Block::Init(const sf::Vector2f& position, const sf::Vector2f& size)
	{
		shape.setSize(size);
		shape.setFillColor(sf::Color::Blue);
		shape.setOutlineColor(sf::Color::White);
		shape.setOutlineThickness(2.f);
		shape.setPosition(position);

		isDestroyed = false;
	}

	void Block::Update(float timeDelta)
	{

	}

	void Block::Draw(sf::RenderWindow& window) const
	{
		if (!isDestroyed)
		{
			window.draw(shape);
		}
	}

	sf::FloatRect Block::GetBounds() const
	{
		return shape.getGlobalBounds();
	}

	bool Block::GetCollision(const Collidable& collidable) const
	{
		return !isDestroyed && Collidable::GetCollision(collidable);
	}

	bool Block::ShouldBounceBall() const
	{
		return true;
	}

	int Block::GetScorePoints() const
	{
		return 10;
	}

	void Block::OnHit(Collidable& collidable)
	{
		Destroy();
	}


	void Block::Destroy()
	{
		if (isDestroyed)
		{
			return;
		}

		isDestroyed = true;
		PlayBlockDestroySound();
		NotifyDestroyed();
	}

	bool Block::IsDestroyed() const
	{
		return isDestroyed;
	}

	void Block::RestoreDestroyedState(bool destroyed)
	{
		isDestroyed = destroyed;
	}

	void Block::AddObserver(IBlockObserver* observer)
	{
		if (observer == nullptr)
		{
			return;
		}

		if (std::find(observers.begin(), observers.end(), observer) == observers.end())
		{
			observers.push_back(observer);
		}
	}

	void Block::RemoveObserver(IBlockObserver* observer)
	{
		auto it = std::remove(observers.begin(), observers.end(), observer);
		observers.erase(it, observers.end());
	}

	void Block::NotifyDestroyed()
	{
		for (IBlockObserver* observer : observers)
		{
			observer->OnBlockDestroyed(*this);
		}
	}
}
