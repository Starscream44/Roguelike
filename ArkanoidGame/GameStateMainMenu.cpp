#include "GameStateMainMenu.h"
#include "Application.h"
#include "Game.h"
#include <assert.h>

namespace Arkanoid
{
	void InitGameStateMainMenu(GameStateMainMenuData& data)
	{
		assert(data.font.loadFromFile(SETTINGS.RESOURCES_PATH + "Fonts/Roboto-Regular.ttf"));
		InitMenuBackground(data.background, SETTINGS.MAIN_MENU_BACKGROUND_PATH, sf::Color(0, 0, 0, 150));
		assert(data.music.openFromFile(SETTINGS.MENU_MUSIC_PATH));
		data.music.setLoop(true);
		data.music.play();

		MenuItem startGame;
		startGame.SetTextString("Start Game");
		startGame.SetTextFont(data.font);
		startGame.SetTextCharacterSize(24);
		startGame.SetOnPressCallback([](MenuItem&) {
			Application::Instance().GetGame().StartGame();
			});

		MenuItem recordsItem;
		recordsItem.SetTextString("Records");
		recordsItem.SetTextFont(data.font);
		recordsItem.SetTextCharacterSize(24);
		recordsItem.SetOnPressCallback([](MenuItem&) {
			Application::Instance().GetGame().ShowRecords();
			});

		MenuItem yesItem;
		yesItem.SetTextString("Yes");
		yesItem.SetTextFont(data.font);
		yesItem.SetTextCharacterSize(24);
		yesItem.SetOnPressCallback([](MenuItem&) {
			Application::Instance().GetGame().ExitGame();
			});

		MenuItem noItem;
		noItem.SetTextString("No");
		noItem.SetTextFont(data.font);
		noItem.SetTextCharacterSize(24);
		noItem.SetOnPressCallback([&data](MenuItem&) {
			data.menu.GoBack();
			});

		MenuItem exitGameItem;
		exitGameItem.SetTextString("Exit Game");
		exitGameItem.SetTextFont(data.font);
		exitGameItem.SetTextCharacterSize(24);
		exitGameItem.SetHintString("Are you sure?");
		exitGameItem.SetHintFont(data.font);
		exitGameItem.SetHintCharacterSize(48);
		exitGameItem.SetHintFillColor(sf::Color::Red);
		exitGameItem.SetChildrenLayout(Orientation::Horizontal, Alignment::Middle, 10.f);
		exitGameItem.AddChild(yesItem);
		exitGameItem.AddChild(noItem);

		MenuItem mainMenu;
		mainMenu.SetHintString("Void Bricks");
		mainMenu.SetHintFont(data.font);
		mainMenu.SetHintCharacterSize(48);
		mainMenu.SetHintFillColor(sf::Color(70, 220, 255));
		mainMenu.SetChildrenLayout(Orientation::Vertical, Alignment::Middle, 10.f);
		mainMenu.AddChild(startGame);
		mainMenu.AddChild(recordsItem);
		mainMenu.AddChild(exitGameItem);

		data.menu.Init(mainMenu);
	}

	void ShutdownGameStateMainMenu(GameStateMainMenuData& data)
	{
		data.music.stop();
	}

	void HandleGameStateMainMenuWindowEvent(GameStateMainMenuData& data, const sf::Event& event)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				data.menu.GoBack();
			}
			else if (event.key.code == sf::Keyboard::Enter)
			{
				data.menu.PressOnSelectedItem();
			}
			
			Orientation orientation = data.menu.GetCurrentContext().GetChildrenOrientation();
			if (orientation == Orientation::Vertical && event.key.code == sf::Keyboard::Up ||
				orientation == Orientation::Horizontal && event.key.code == sf::Keyboard::Left)
			{
				data.menu.SwitchToPreviousMenuItem();
			}
			else if (orientation == Orientation::Vertical && event.key.code == sf::Keyboard::Down ||
						orientation == Orientation::Horizontal && event.key.code == sf::Keyboard::Right)
			{
				data.menu.SwitchToNextMenuItem();
			}
		}
	}

	void UpdateGameStateMainMenu(GameStateMainMenuData& data, float timeDelta)
	{

	}

	void DrawGameStateMainMenu(GameStateMainMenuData& data, sf::RenderWindow& window)
	{
		sf::Vector2f viewSize = (sf::Vector2f)window.getView().getSize();

		DrawMenuBackground(data.background, window);

		sf::Text hintText = data.menu.GetCurrentContext().GetHintText();
		hintText.setOrigin(GetTextOrigin(hintText, { 0.5f, 0.f }));
		hintText.setPosition(viewSize.x / 2.f, 150.f);
		window.draw(hintText);

		data.menu.Draw(window, viewSize / 2.f, { 0.5f, 0.f });
	}

}
