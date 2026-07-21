#pragma once

#include "GameObject.h"
#include "Collidable.h"

#include <SFML/Graphics.hpp>

#include <vector>

namespace Arkanoid
{
	class IBlockObserver;

	class Block : public GameObject, public Collidable
	{
	public:
		virtual ~Block() = default;

		virtual void Init(const sf::Vector2f& position, const sf::Vector2f& size);

		void Update(float timeDelta) override;
		void Draw(sf::RenderWindow& window) const override;

		sf::FloatRect GetBounds() const override;
		bool GetCollision(const Collidable& collidable) const override;

		virtual bool ShouldBounceBall() const;
		virtual int GetScorePoints() const;

		void Destroy();
		bool IsDestroyed() const;
		void RestoreDestroyedState(bool destroyed);
		void AddObserver(IBlockObserver* observer);
		void RemoveObserver(IBlockObserver* observer);

	protected:
		void OnHit(Collidable& collidable) override;

	private:
		void NotifyDestroyed();

	protected:
		sf::RectangleShape shape;
		bool isDestroyed = false;

	private:
		std::vector<IBlockObserver*> observers;
	};
}
