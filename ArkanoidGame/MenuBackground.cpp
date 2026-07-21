#include "MenuBackground.h"

#include <algorithm>
#include <assert.h>

namespace Arkanoid
{
	void InitMenuBackground(MenuBackground& background, const std::string& texturePath, sf::Color overlayColor)
	{
		assert(background.texture.loadFromFile(texturePath));
		background.texture.setSmooth(true);
		background.sprite.setTexture(background.texture);
		background.overlay.setFillColor(overlayColor);
	}

	void DrawMenuBackground(MenuBackground& background, sf::RenderWindow& window)
	{
		const sf::Vector2f viewSize = window.getView().getSize();
		const sf::Vector2u textureSize = background.texture.getSize();
		const float scale = std::max(
			viewSize.x / static_cast<float>(textureSize.x),
			viewSize.y / static_cast<float>(textureSize.y)
		);

		background.sprite.setScale(scale, scale);
		background.sprite.setPosition(
			(viewSize.x - textureSize.x * scale) * 0.5f,
			(viewSize.y - textureSize.y * scale) * 0.5f
		);
		window.draw(background.sprite);

		background.overlay.setSize(viewSize);
		background.overlay.setPosition(0.f, 0.f);
		window.draw(background.overlay);
	}
}
