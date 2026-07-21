#pragma once
#include <SFML/Audio.hpp>
#include "SFML/Graphics.hpp"
#include "MenuBackground.h"
#include <vector>

namespace Arkanoid
{
	class Game;

	class GameStateGameOverData
	{
		friend void InitGameStateGameOver(GameStateGameOverData& data);
		friend void ShutdownGameStateGameOver(GameStateGameOverData& data);
		friend void HandleGameStateGameOverWindowEvent(GameStateGameOverData& data, const sf::Event& event);
		friend void UpdateGameStateGameOver(GameStateGameOverData& data, float timeDelta);
		friend void DrawGameStateGameOver(GameStateGameOverData& data, sf::RenderWindow& window);

	private:
		// Resources
		sf::Font font;
		sf::Music music;

		float timeSinceGameOver = 0.f;

		// UI data
		MenuBackground background;
		sf::Text gameOverText;
		std::vector<sf::Text> hintTexts;
		std::vector<sf::Text> recordsTableTexts;
	};

	void InitGameStateGameOver(GameStateGameOverData& data);
	void ShutdownGameStateGameOver(GameStateGameOverData& data);
	void HandleGameStateGameOverWindowEvent(GameStateGameOverData& data, const sf::Event& event);
	void UpdateGameStateGameOver(GameStateGameOverData& data, float timeDelta);
	void DrawGameStateGameOver(GameStateGameOverData& data, sf::RenderWindow& window);
}
