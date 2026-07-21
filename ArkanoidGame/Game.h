#pragma once
#include <SFML/Graphics.hpp>

#include "GameSettings.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace Arkanoid
{
	enum class GameStateType
	{
		None = 0,
		MainMenu,
		Playing,
		GameOver,
		Victory,
		ExitDialog,
		Records,
	};

	class GameState
	{
	public:
		GameState() = default;
		GameState(GameStateType type, bool isExclusivelyVisible);
		~GameState();

		GameState(const GameState&) = delete;
		GameState& operator=(const GameState&) = delete;

		GameState(GameState&& other) noexcept;
		GameState& operator=(GameState&& other) noexcept;

		GameStateType GetType() const { return type; }
		bool IsExclusivelyVisible() const { return isExclusivelyVisible; }

	private:
		friend void HandleWindowEventGameState(GameState& state, sf::Event& event);
		friend void UpdateGameState(GameState& state, float timeDelta);
		friend void DrawGameState(GameState& state, sf::RenderWindow& window);

		void CreateStateData();
		void ShutdownAndDestroyStateData();

	private:
		GameStateType type = GameStateType::None;
		void* data = nullptr;
		bool isExclusivelyVisible = false;
	};

	enum class GameStateChangeType
	{
		None,
		Push,
		Pop,
		Switch
	};

	class Game
	{
	public:
		using RecordsTable = std::unordered_map<std::string, int>;

		Game();
		~Game();

		void StartGame();
		void PauseGame();
		void ResumeGame();
		void WinGame();
		void LoseGame();
		void ExitGame();
		void ShowRecords();
		void ShowMainMenu();
		void UpdateGame(float timeDelta, sf::RenderWindow& window);

		const RecordsTable& GetRecordsTable() const { return recordsTable; }
		int GetRecordByPlayerId(const std::string& playerId) const;
		int GetLastScore() const { return lastScore; }
		void UpdateRecord(const std::string& playerId, int score);

	private:
		void LoadRecords();
		void SaveRecords() const;

		void HandleWindowEvents(sf::RenderWindow& window);
		bool Update(float timeDelta); // Return false if game should be closed
		void Draw(sf::RenderWindow& window);
		void Shutdown();

		// Add new game state on top of the stack
		void PushState(GameStateType stateType, bool isExclusivelyVisible);

		// Remove current game state from the stack
		void PopState();

		// Remove all game states from the stack and add new one
		void SwitchStateTo(GameStateType newState);

		std::vector<GameState> stateStack;
		GameStateChangeType stateChangeType = GameStateChangeType::None;
		GameStateType pendingGameStateType = GameStateType::None;
		bool pendingGameStateIsExclusivelyVisible = false;

		RecordsTable recordsTable;
		int lastScore = 0;
	};

	void HandleWindowEventGameState(GameState& state, sf::Event& event);
	void UpdateGameState(GameState& state, float timeDelta);
	void DrawGameState(GameState& state, sf::RenderWindow& window);
}
