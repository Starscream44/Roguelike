#pragma once
#include <SFML/Graphics.hpp>
#include "MenuBackground.h"
#include <vector>

namespace Arkanoid
{
	class Game;

	class GameStateRecordsData
	{
		friend void InitGameStateRecords(GameStateRecordsData& data);
		friend void ShutdownGameStateRecords(GameStateRecordsData& data);
		friend void HandleGameStateRecordsWindowEvent(GameStateRecordsData& data, const sf::Event& event);
		friend void UpdateGameStateRecords(GameStateRecordsData& data, float timeDelta);
		friend void DrawGameStateRecords(GameStateRecordsData& data, sf::RenderWindow& window);

	private:
		// Resources
		sf::Font font;
		MenuBackground background;

		sf::Text titleText;
		std::vector<sf::Text> tableTexts;
		sf::Text hintText;
	};

	void InitGameStateRecords(GameStateRecordsData& data);
	void ShutdownGameStateRecords(GameStateRecordsData& data);
	void HandleGameStateRecordsWindowEvent(GameStateRecordsData& data, const sf::Event& event);
	void UpdateGameStateRecords(GameStateRecordsData& data, float timeDelta);
	void DrawGameStateRecords(GameStateRecordsData& data, sf::RenderWindow& window);
}
