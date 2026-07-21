#include "Game.h"
#include <assert.h>
#include <algorithm>
#include "GameStatePlaying.h"
#include "GameStateGameOver.h"
#include "GameStatePauseMenu.h"
#include "GameStateMainMenu.h"
#include "GameStateVictory.h"
#include "GameStateRecords.h"
#include <fstream>

namespace Arkanoid
{

	GameState::GameState(GameStateType type, bool isExclusivelyVisible)
		: type(type)
		, isExclusivelyVisible(isExclusivelyVisible)
	{
		CreateStateData();
	}

	GameState::~GameState()
	{
		ShutdownAndDestroyStateData();
	}

	GameState::GameState(GameState&& other) noexcept
		: type(other.type)
		, data(other.data)
		, isExclusivelyVisible(other.isExclusivelyVisible)
	{
		other.type = GameStateType::None;
		other.data = nullptr;
		other.isExclusivelyVisible = false;
	}

	GameState& GameState::operator=(GameState&& other) noexcept
	{
		if (this != &other)
		{
			ShutdownAndDestroyStateData();

			type = other.type;
			data = other.data;
			isExclusivelyVisible = other.isExclusivelyVisible;

			other.type = GameStateType::None;
			other.data = nullptr;
			other.isExclusivelyVisible = false;
		}

		return *this;
	}

	void GameState::CreateStateData()
	{
		switch (type)
		{
		case GameStateType::MainMenu:
		{
			data = new GameStateMainMenuData();
			InitGameStateMainMenu(*(GameStateMainMenuData*)data);
			break;
		}
		case GameStateType::Playing:
		{
			data = new GameStatePlayingData();
			InitGameStatePlaying(*(GameStatePlayingData*)data);
			break;
		}
		case GameStateType::GameOver:
		{
			data = new GameStateGameOverData();
			InitGameStateGameOver(*(GameStateGameOverData*)data);
			break;
		}

		case GameStateType::Victory:
		{
			data = new GameStateVictoryData();
			InitGameStateVictory(*(GameStateVictoryData*)data);
			break;
		}

		case GameStateType::ExitDialog:
		{
			data = new GameStatePauseMenuData();
			InitGameStatePauseMenu(*(GameStatePauseMenuData*)data);
			break;
		}
		case GameStateType::Records:
		{
			data = new GameStateRecordsData();
			InitGameStateRecords(*(GameStateRecordsData*)data);
			break;
		}
		case GameStateType::None:
		{
			data = nullptr;
			break;
		}
		default:
			assert(false);
			break;
		}
	}

	void GameState::ShutdownAndDestroyStateData()
	{
		if (data == nullptr)
		{
			return;
		}

		switch (type)
		{
		case GameStateType::MainMenu:
		{
			ShutdownGameStateMainMenu(*(GameStateMainMenuData*)data);
			delete (GameStateMainMenuData*)data;
			break;
		}
		case GameStateType::Playing:
		{
			ShutdownGameStatePlaying(*(GameStatePlayingData*)data);
			delete (GameStatePlayingData*)data;
			break;
		}
		case GameStateType::GameOver:
		{
			ShutdownGameStateGameOver(*(GameStateGameOverData*)data);
			delete (GameStateGameOverData*)data;
			break;
		}

		case GameStateType::Victory:
		{
			ShutdownGameStateVictory(*(GameStateVictoryData*)data);
			delete (GameStateVictoryData*)data;
			break;
		}

		case GameStateType::ExitDialog:
		{
			ShutdownGameStatePauseMenu(*(GameStatePauseMenuData*)data);
			delete (GameStatePauseMenuData*)data;
			break;
		}
		case GameStateType::Records:
		{
			ShutdownGameStateRecords(*(GameStateRecordsData*)data);
			delete (GameStateRecordsData*)data;
			break;
		}
		case GameStateType::None:
		{
			break;
		}
		default:
			assert(false);
			break;
		}

		data = nullptr;
		type = GameStateType::None;
		isExclusivelyVisible = false;
	}

	Game::Game()
	{
		LoadRecords();

		stateChangeType = GameStateChangeType::None;
		pendingGameStateType = GameStateType::None;
		pendingGameStateIsExclusivelyVisible = false;
		SwitchStateTo(GameStateType::MainMenu);
	}

	Game::~Game()
	{
		Shutdown();
	}

	void Game::StartGame()
	{
		SwitchStateTo(GameStateType::Playing);
	}

	void Game::PauseGame()
	{
		PushState(GameStateType::ExitDialog, false);
	}

	void Game::ResumeGame()
	{
		PopState();
	}

	void Game::WinGame()
	{
		SwitchStateTo(GameStateType::Victory);
	}

	void Game::LoseGame()
	{
		SwitchStateTo(GameStateType::GameOver);
	}

	void Game::ExitGame()
	{
		SwitchStateTo(GameStateType::None);
	}

	void Game::ShowRecords()
	{
		PushState(GameStateType::Records, true);
	}

	void Game::ShowMainMenu()
	{
		SwitchStateTo(GameStateType::MainMenu);
	}

	void Game::UpdateGame(float timeDelta, sf::RenderWindow& window)
	{
		HandleWindowEvents(window);

		if (!window.isOpen())
		{
			return;
		}

		if (Update(timeDelta))
		{
			window.clear();
			Draw(window);
			window.display();
		}
		else
		{
			window.close();
		}
	}

	void Game::HandleWindowEvents(sf::RenderWindow& window)
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window if close button or Escape key pressed
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			if (stateStack.size() > 0)
			{
				HandleWindowEventGameState(stateStack.back(), event);
			}
		}
	}

	bool Game::Update(float timeDelta)
	{
		if (stateChangeType == GameStateChangeType::Switch)
		{
			while (stateStack.size() > 0)
			{
				stateStack.pop_back();
			}
		}
		else if (stateChangeType == GameStateChangeType::Pop)
		{
			if (stateStack.size() > 0)
			{
				stateStack.pop_back();
			}
		}

		// Initialize new game state if needed
		if (pendingGameStateType != GameStateType::None)
		{
			stateStack.emplace_back(pendingGameStateType, pendingGameStateIsExclusivelyVisible);
		}

		stateChangeType = GameStateChangeType::None;
		pendingGameStateType = GameStateType::None;
		pendingGameStateIsExclusivelyVisible = false;

		if (stateStack.size() > 0)
		{
			UpdateGameState(stateStack.back(), timeDelta);
			return true;
		}

		return false;
	}

	void Game::Draw(sf::RenderWindow& window)
	{
		if (stateStack.size() > 0)
		{
			std::vector<GameState*> visibleGameStates;
			for (auto it = stateStack.rbegin(); it != stateStack.rend(); ++it)
			{
				visibleGameStates.push_back(&(*it));
				if (it->IsExclusivelyVisible())
				{
					break;
				}
			}

			for (auto it = visibleGameStates.rbegin(); it != visibleGameStates.rend(); ++it)
			{
				DrawGameState(**it, window);
			}
		}
	}

	void Game::Shutdown()
	{
		// Shutdown all game states
		while (stateStack.size() > 0)
		{
			stateStack.pop_back();
		}

		stateChangeType = GameStateChangeType::None;
		pendingGameStateType = GameStateType::None;
		pendingGameStateIsExclusivelyVisible = false;
	}

	void Game::PushState(GameStateType stateType, bool isExclusivelyVisible)
	{
		pendingGameStateType = stateType;
		pendingGameStateIsExclusivelyVisible = isExclusivelyVisible;
		stateChangeType = GameStateChangeType::Push;
	}

	void Game::PopState()
	{
		pendingGameStateType = GameStateType::None;
		pendingGameStateIsExclusivelyVisible = false;
		stateChangeType = GameStateChangeType::Pop;
	}

	void Game::SwitchStateTo(GameStateType newState)
	{
		pendingGameStateType = newState;
		pendingGameStateIsExclusivelyVisible = false;
		stateChangeType = GameStateChangeType::Switch;
	}

	int Game::GetRecordByPlayerId(const std::string& playerId) const
	{
		auto it = recordsTable.find(playerId);
		return it == recordsTable.end() ? 0 : it->second;
	}

	void Game::UpdateRecord(const std::string& playerId, int score)
	{
		lastScore = score;
		recordsTable[playerId] = std::max(recordsTable[playerId], score);
		SaveRecords();
	}

	void Game::LoadRecords()
	{
		recordsTable.clear();

		std::ifstream file(SETTINGS.RECORDS_FILE_PATH);
		if (file.is_open())
		{
			std::string playerId;
			int score = 0;
			while (file >> playerId >> score)
			{
				recordsTable[playerId] = score;
			}
		}

		if (!recordsTable.empty())
		{
			return;
		}

		recordsTable =
		{
			{"John", 120},
			{"Jane", 90},
			{"Alice", 60},
			{"Bob", 40},
			{"Clementine", 30},
		};
	}

	void Game::SaveRecords() const
	{
		std::ofstream file(SETTINGS.RECORDS_FILE_PATH);
		if (!file.is_open())
		{
			return;
		}

		for (const auto& item : recordsTable)
		{
			file << item.first << ' ' << item.second << '\n';
		}
	}

	void HandleWindowEventGameState(GameState& state, sf::Event& event)
	{
		switch (state.type)
		{
		case GameStateType::MainMenu:
		{
			HandleGameStateMainMenuWindowEvent(*(GameStateMainMenuData*)state.data, event);
			break;
		}
		case GameStateType::Playing:
		{
			HandleGameStatePlayingWindowEvent(*(GameStatePlayingData*)state.data, event);
			break;
		}
		case GameStateType::GameOver:
		{
			HandleGameStateGameOverWindowEvent(*(GameStateGameOverData*)state.data, event);
			break;
		}
		case GameStateType::Victory:
		{
			HandleGameStateVictoryWindowEvent(*(GameStateVictoryData*)state.data, event);
			break;
		}
		case GameStateType::ExitDialog:
		{
			HandleGameStatePauseMenuWindowEvent(*(GameStatePauseMenuData*)state.data, event);
			break;
		}
		case GameStateType::Records:
		{
			HandleGameStateRecordsWindowEvent(*(GameStateRecordsData*)state.data, event);
			break;
		}
		default:
			assert(false); // We want to know if we forgot to implement new game state
			break;
		}
	}

	void UpdateGameState(GameState& state, float timeDelta)
	{
		switch (state.type)
		{
		case GameStateType::MainMenu:
		{
			UpdateGameStateMainMenu(*(GameStateMainMenuData*)state.data, timeDelta);
			break;
		}
		case GameStateType::Playing:
		{
			UpdateGameStatePlaying(*(GameStatePlayingData*)state.data, timeDelta);
			break;
		}
		case GameStateType::GameOver:
		{
			UpdateGameStateGameOver(*(GameStateGameOverData*)state.data, timeDelta);
			break;
		}
		case GameStateType::Victory:
		{
			UpdateGameStateVictory(*(GameStateVictoryData*)state.data, timeDelta);
			break;
		}
		case GameStateType::ExitDialog:
		{
			UpdateGameStatePauseMenu(*(GameStatePauseMenuData*)state.data, timeDelta);
			break;
		}
		case GameStateType::Records:
		{
			UpdateGameStateRecords(*(GameStateRecordsData*)state.data, timeDelta);
			break;
		}
		default:
			assert(false); // We want to know if we forgot to implement new game state
			break;
		}
	}

	void DrawGameState(GameState& state, sf::RenderWindow& window)
	{
		switch (state.type)
		{
		case GameStateType::MainMenu:
		{
			DrawGameStateMainMenu(*(GameStateMainMenuData*)state.data, window);
			break;
		}
		case GameStateType::Playing:
		{
			DrawGameStatePlaying(*(GameStatePlayingData*)state.data, window);
			break;
		}
		case GameStateType::GameOver:
		{
			DrawGameStateGameOver(*(GameStateGameOverData*)state.data, window);
			break;
		}
		case GameStateType::Victory:
		{
			DrawGameStateVictory(*(GameStateVictoryData*)state.data, window);
			break;
		}
		case GameStateType::ExitDialog:
		{
			DrawGameStatePauseMenu(*(GameStatePauseMenuData*)state.data, window);
			break;
		}
		case GameStateType::Records:
		{
			DrawGameStateRecords(*(GameStateRecordsData*)state.data, window);
			break;
		}
		default:
			assert(false); // We want to know if we forgot to implement new game state
			break;
		}
	}
}
