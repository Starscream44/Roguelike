#pragma once

#include <SFML/Graphics.hpp>

#include <string>

namespace Arkanoid
{
	struct MenuBackground
	{
		sf::Texture texture;
		sf::Sprite sprite;
		sf::RectangleShape overlay;
	};

	void InitMenuBackground(MenuBackground& background, const std::string& texturePath, sf::Color overlayColor);
	void DrawMenuBackground(MenuBackground& background, sf::RenderWindow& window);
}
