#include "GameStateGameOver.h"
#include "Application.h"
#include "Game.h"
#include "Text.h"
#include <assert.h>
#include <map>
#include <sstream>
#include <string>

namespace Arkanoid
{
	void InitGameStateGameOver(GameStateGameOverData& data)
	{
		assert(data.font.loadFromFile(SETTINGS.RESOURCES_PATH + "Fonts/Roboto-Regular.ttf"));

		data.timeSinceGameOver = 0.f;
		InitMenuBackground(data.background, SETTINGS.MAIN_MENU_BACKGROUND_PATH, sf::Color(0, 0, 0, 185));
		assert(data.music.openFromFile(SETTINGS.MENU_MUSIC_PATH));
		data.music.setLoop(true);
		data.music.play();

		data.gameOverText.setFont(data.font);
		data.gameOverText.setCharacterSize(48);
		data.gameOverText.setStyle(sf::Text::Bold);
		data.gameOverText.setFillColor(sf::Color::Red);
		data.gameOverText.setString("GAME OVER");
		
		data.recordsTableTexts.reserve(SETTINGS.MAX_RECORDS_TABLE_SIZE);

		std::multimap<int, std::string> sortedRecordsTable;
		Game& game = Application::Instance().GetGame();
		for (const auto& item : game.GetRecordsTable())
		{
			sortedRecordsTable.insert(std::make_pair(item.second, item.first));
		}

		bool isPlayerInTable = false;
		auto it = sortedRecordsTable.rbegin();
		for (int i = 0; i < SETTINGS.MAX_RECORDS_TABLE_SIZE && it != sortedRecordsTable.rend(); ++i, ++it) // Note, we can do several actions in for action block
		{
			data.recordsTableTexts.emplace_back(); // Create text in place
			sf::Text& text = data.recordsTableTexts.back();

			// We can use streams for writing into string and reading from it
			std::stringstream sstream;
			sstream << i + 1 << ". " << it->second << ": " << it->first;
			text.setString(sstream.str());
			text.setFont(data.font);
			text.setCharacterSize(24);
			if (it->second == SETTINGS.PLAYER_NAME)
			{
				text.setFillColor(sf::Color::Green);
				isPlayerInTable = true;
			}
			else
			{
				text.setFillColor(sf::Color::White);
			}
		}

		// If player is not in table, replace last element with him
		if (!isPlayerInTable)
		{
			sf::Text& text = data.recordsTableTexts.back();
			std::stringstream sstream;
			int playerScore = game.GetRecordByPlayerId(SETTINGS.PLAYER_NAME);
			sstream << SETTINGS.MAX_RECORDS_TABLE_SIZE << ". " << SETTINGS.PLAYER_NAME << ": " << playerScore;
			text.setString(sstream.str());
			text.setFillColor(sf::Color::Green);
		}

		const std::vector<std::string> hintLines =
		{
			"Score: " + std::to_string(game.GetLastScore()),
			"Press Space to restart",
			"Esc to exit to main menu"
		};

		data.hintTexts.clear();
		data.hintTexts.reserve(hintLines.size());
		for (const std::string& hintLine : hintLines)
		{
			data.hintTexts.emplace_back();
			sf::Text& text = data.hintTexts.back();
			text.setFont(data.font);
			text.setCharacterSize(24);
			text.setFillColor(sf::Color::White);
			text.setString(hintLine);
		}
	}

	void ShutdownGameStateGameOver(GameStateGameOverData& data)
	{
		data.music.stop();
	}

	void HandleGameStateGameOverWindowEvent(GameStateGameOverData& data, const sf::Event& event)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Space)
			{
				Application::Instance().GetGame().StartGame();
			}
			else if (event.key.code == sf::Keyboard::Escape)
			{
				Application::Instance().GetGame().ShowMainMenu();
			}
		}
	}

	void UpdateGameStateGameOver(GameStateGameOverData& data, float timeDelta)
	{
		data.timeSinceGameOver += timeDelta;

		sf::Color gameOverTextColor = (int)data.timeSinceGameOver % 2 ? sf::Color::Red : sf::Color::Yellow;
		data.gameOverText.setFillColor(gameOverTextColor);

	}

	void DrawGameStateGameOver(GameStateGameOverData& data, sf::RenderWindow& window)
	{
		sf::Vector2f viewSize = window.getView().getSize();

		DrawMenuBackground(data.background, window);

		data.gameOverText.setOrigin(GetTextOrigin(data.gameOverText, { 0.5f, 1.f }));
		data.gameOverText.setPosition(viewSize.x / 2.f, viewSize.y / 2 - 50.f);
		window.draw(data.gameOverText);

		// We need to create new vector here as DrawItemsList needs vector of pointers
		std::vector<sf::Text*> textsList;
		textsList.reserve(data.recordsTableTexts.size());
		for (auto& text : data.recordsTableTexts)
		{
			textsList.push_back(&text);
		}

		sf::Vector2f tablePosition = { viewSize.x / 2, viewSize.y / 2.f };
		DrawTextList(window, textsList, 10.f, Orientation::Vertical, Alignment::Min, tablePosition, { 0.5f, 0.f });

		std::vector<sf::Text*> hintTextsList;
		hintTextsList.reserve(data.hintTexts.size());
		for (auto& text : data.hintTexts)
		{
			hintTextsList.push_back(&text);
		}

		DrawTextList(window, hintTextsList, 6.f, Orientation::Vertical, Alignment::Middle, { viewSize.x / 2.f, viewSize.y - 50.f }, { 0.5f, 1.f });
	}
}
