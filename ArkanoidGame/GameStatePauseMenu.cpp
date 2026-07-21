#include "GameStatePauseMenu.h"
#include "Application.h"
#include "Game.h"
#include <assert.h>

namespace Arkanoid
{
	void InitGameStatePauseMenu(GameStatePauseMenuData& data)
	{
		assert(data.font.loadFromFile(SETTINGS.RESOURCES_PATH + "Fonts/Roboto-Regular.ttf"));

		data.background.setFillColor(sf::Color(0, 0, 0, 128));

		data.titleText.setString("Pause");
		data.titleText.setFont(data.font);
		data.titleText.setCharacterSize(48);
		data.titleText.setFillColor(sf::Color::Red);

		MenuItem resumeItem;
		resumeItem.SetTextString("Return to game");
		resumeItem.SetTextFont(data.font);
		resumeItem.SetTextCharacterSize(24);
		resumeItem.SetOnPressCallback([](MenuItem&) {
			Application::Instance().GetGame().ResumeGame();
			});

		MenuItem exitItem;
		exitItem.SetTextString("Exit to main menu");
		exitItem.SetTextFont(data.font);
		exitItem.SetTextCharacterSize(24);
		exitItem.SetOnPressCallback([](MenuItem&) {
			Application::Instance().GetGame().ShowMainMenu();
			});

		MenuItem pauseMenu;
		pauseMenu.SetChildrenLayout(Orientation::Vertical, Alignment::Middle, 0.f);
		pauseMenu.AddChild(resumeItem);
		pauseMenu.AddChild(exitItem);

		data.menu.Init(pauseMenu);
	}

	void ShutdownGameStatePauseMenu(GameStatePauseMenuData& data)
	{
		// We dont need to free resources here, because they will be freed automatically
	}

	void HandleGameStatePauseMenuWindowEvent(GameStatePauseMenuData& data, const sf::Event& event)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				Application::Instance().GetGame().ResumeGame();
			}

			if (event.key.code == sf::Keyboard::Enter)
			{
				data.menu.PressOnSelectedItem();
			}

			Orientation orientation = data.menu.GetCurrentContext().GetChildrenOrientation();
			if (event.key.code == sf::Keyboard::Up)
			{
				data.menu.SwitchToPreviousMenuItem();
			}
			else if (event.key.code == sf::Keyboard::Down)
			{
				data.menu.SwitchToNextMenuItem();
			}
		}
	}

	void UpdateGameStatePauseMenu(GameStatePauseMenuData& data, float timeDelta)
	{

	}

	void DrawGameStatePauseMenu(GameStatePauseMenuData& data, sf::RenderWindow& window)
	{
		sf::Vector2f viewSize = (sf::Vector2f)window.getView().getSize();
		
		data.background.setSize(viewSize);
		window.draw(data.background);

		data.titleText.setOrigin(GetTextOrigin(data.titleText, { 0.5f, 0.f }));
		data.titleText.setPosition(viewSize.x / 2.f, 100);
		window.draw(data.titleText);

		data.menu.Draw(window, window.getView().getCenter(), { 0.5f, 0.f });
	}

}
