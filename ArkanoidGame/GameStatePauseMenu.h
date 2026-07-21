#pragma once
#include <SFML/Graphics.hpp>
#include "Menu.h"

namespace Arkanoid
{
	class Game;

	class GameStatePauseMenuData
	{
		friend void InitGameStatePauseMenu(GameStatePauseMenuData& data);
		friend void ShutdownGameStatePauseMenu(GameStatePauseMenuData& data);
		friend void HandleGameStatePauseMenuWindowEvent(GameStatePauseMenuData& data, const sf::Event& event);
		friend void UpdateGameStatePauseMenu(GameStatePauseMenuData& data, float timeDelta);
		friend void DrawGameStatePauseMenu(GameStatePauseMenuData& data, sf::RenderWindow& window);

	private:
		// Resources
		sf::Font font;

		// UI data
		sf::RectangleShape background;
		sf::Text titleText;
		Menu menu;
	};

	void InitGameStatePauseMenu(GameStatePauseMenuData& data);
	void ShutdownGameStatePauseMenu(GameStatePauseMenuData& data);
	void HandleGameStatePauseMenuWindowEvent(GameStatePauseMenuData& data, const sf::Event& event);
	void UpdateGameStatePauseMenu(GameStatePauseMenuData& data, float timeDelta);
	void DrawGameStatePauseMenu(GameStatePauseMenuData& data, sf::RenderWindow& window);
}