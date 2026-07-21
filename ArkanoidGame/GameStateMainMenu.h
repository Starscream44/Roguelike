#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "Menu.h"
#include "MenuBackground.h"

namespace Arkanoid
{
	class Game;

	class GameStateMainMenuData
	{
		friend void InitGameStateMainMenu(GameStateMainMenuData& data);
		friend void ShutdownGameStateMainMenu(GameStateMainMenuData& data);
		friend void HandleGameStateMainMenuWindowEvent(GameStateMainMenuData& data, const sf::Event& event);
		friend void UpdateGameStateMainMenu(GameStateMainMenuData& data, float timeDelta);
		friend void DrawGameStateMainMenu(GameStateMainMenuData& data, sf::RenderWindow& window);

	private:
		// Resources
		sf::Font font;
		sf::Music music;
		MenuBackground background;

		// UI data
		Menu menu;
	};

	void InitGameStateMainMenu(GameStateMainMenuData& data);
	void ShutdownGameStateMainMenu(GameStateMainMenuData& data);
	void HandleGameStateMainMenuWindowEvent(GameStateMainMenuData& data, const sf::Event& event);
	void UpdateGameStateMainMenu(GameStateMainMenuData& data, float timeDelta);
	void DrawGameStateMainMenu(GameStateMainMenuData& data, sf::RenderWindow& window);
}
