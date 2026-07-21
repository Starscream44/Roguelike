#include "GameStateVictory.h"
#include "Application.h"
#include "Game.h"
#include "GameSettings.h"
#include "Text.h"

#include <assert.h>
#include <string>

namespace Arkanoid
{
	void InitGameStateVictory(GameStateVictoryData& data)
	{
		assert(data.font.loadFromFile(SETTINGS.RESOURCES_PATH + "Fonts/Roboto-Regular.ttf"));

		InitMenuBackground(data.background, SETTINGS.MAIN_MENU_BACKGROUND_PATH, sf::Color(0, 0, 0, 180));
		assert(data.music.openFromFile(SETTINGS.MENU_MUSIC_PATH));
		data.music.setLoop(true);
		data.music.play();

		data.titleText.setFont(data.font);
		data.titleText.setCharacterSize(48);
		data.titleText.setStyle(sf::Text::Bold);
		data.titleText.setFillColor(sf::Color::Green);
		data.titleText.setString("VICTORY!");

		data.hintText.setFont(data.font);
		data.hintText.setCharacterSize(26);
		data.hintText.setFillColor(sf::Color::White);
		const Game& game = Application::Instance().GetGame();
		data.hintText.setString("Congratulations!\nYou destroyed all blocks.\nScore: " + std::to_string(game.GetLastScore()) + "\n\nPlay again?");

		MenuItem yesItem;
		yesItem.SetTextString("Yes");
		yesItem.SetTextFont(data.font);
		yesItem.SetTextCharacterSize(28);
		yesItem.SetOnPressCallback([](MenuItem&) {
			Application::Instance().GetGame().StartGame();
			});

		MenuItem noItem;
		noItem.SetTextString("No");
		noItem.SetTextFont(data.font);
		noItem.SetTextCharacterSize(28);
		noItem.SetOnPressCallback([](MenuItem&) {
			Application::Instance().GetGame().ShowMainMenu();
			});

		MenuItem victoryMenu;
		victoryMenu.SetChildrenLayout(Orientation::Horizontal, Alignment::Middle, 30.f);
		victoryMenu.AddChild(yesItem);
		victoryMenu.AddChild(noItem);

		data.menu.Init(victoryMenu);
	}

	void ShutdownGameStateVictory(GameStateVictoryData& data)
	{
		data.music.stop();
	}

	void HandleGameStateVictoryWindowEvent(GameStateVictoryData& data, const sf::Event& event)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Enter)
			{
				data.menu.PressOnSelectedItem();
			}
			else if (event.key.code == sf::Keyboard::Left)
			{
				data.menu.SwitchToPreviousMenuItem();
			}
			else if (event.key.code == sf::Keyboard::Right)
			{
				data.menu.SwitchToNextMenuItem();
			}
			else if (event.key.code == sf::Keyboard::Escape)
			{
				Application::Instance().GetGame().ShowMainMenu();
			}
		}
	}

	void UpdateGameStateVictory(GameStateVictoryData& data, float timeDelta)
	{
		data.menu.Update(timeDelta);
	}

	void DrawGameStateVictory(GameStateVictoryData& data, sf::RenderWindow& window)
	{
		const sf::Vector2f viewSize = window.getView().getSize();

		DrawMenuBackground(data.background, window);

		data.titleText.setOrigin(GetTextOrigin(data.titleText, { 0.5f, 0.f }));
		data.titleText.setPosition(viewSize.x / 2.f, 110.f);
		window.draw(data.titleText);

		data.hintText.setOrigin(GetTextOrigin(data.hintText, { 0.5f, 0.f }));
		data.hintText.setPosition(viewSize.x / 2.f, 220.f);
		window.draw(data.hintText);

		data.menu.Draw(window, { viewSize.x / 2.f, 420.f }, { 0.5f, 0.f });
	}
}
