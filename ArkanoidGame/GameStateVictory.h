#pragma once

#include <SFML/Audio.hpp>
#include "SFML/Graphics.hpp"
#include "Menu.h"
#include "MenuBackground.h"

namespace Arkanoid
{
	class Game;

	class GameStateVictoryData
	{
		friend void InitGameStateVictory(GameStateVictoryData& data);
		friend void ShutdownGameStateVictory(GameStateVictoryData& data);
		friend void HandleGameStateVictoryWindowEvent(GameStateVictoryData& data, const sf::Event& event);
		friend void UpdateGameStateVictory(GameStateVictoryData& data, float timeDelta);
		friend void DrawGameStateVictory(GameStateVictoryData& data, sf::RenderWindow& window);

	private:
		sf::Font font;
		sf::Music music;

		MenuBackground background;
		sf::Text titleText;
		sf::Text hintText;

		Menu menu;
	};

	void InitGameStateVictory(GameStateVictoryData& data);
	void ShutdownGameStateVictory(GameStateVictoryData& data);
	void HandleGameStateVictoryWindowEvent(GameStateVictoryData& data, const sf::Event& event);
	void UpdateGameStateVictory(GameStateVictoryData& data, float timeDelta);
	void DrawGameStateVictory(GameStateVictoryData& data, sf::RenderWindow& window);
}
