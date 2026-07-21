#pragma once
#include <SFML/Audio.hpp>
#include "SFML/Graphics.hpp"
#include "Paddle.h"
#include "Ball.h"
#include "Block.h"
#include "BlockFactory.h"
#include "BlocksDestroyObserver.h"
#include "Bonus.h"
#include "LevelLoader.h"
#include "MenuBackground.h"

#include <vector>
#include <memory>
#include <unordered_map>

namespace Arkanoid
{
	class Game;

	class GameStatePlayingData
	{
		friend void InitGameStatePlaying(GameStatePlayingData& data);
		friend void ShutdownGameStatePlaying(GameStatePlayingData& data);
		friend void HandleGameStatePlayingWindowEvent(GameStatePlayingData& data, const sf::Event& event);
		friend void UpdateGameStatePlaying(GameStatePlayingData& data, float timeDelta);
		friend void DrawGameStatePlaying(GameStatePlayingData& data, sf::RenderWindow& window);
		friend void InitBlockFactories(GameStatePlayingData& data);
		friend void LoadGameLevel(GameStatePlayingData& data, int levelIndex);
		friend void SaveGameState(GameStatePlayingData& data);
		friend bool LoadSavedGameState(GameStatePlayingData& data);
		friend void ClearActiveBonusEffects(GameStatePlayingData& data);
		friend void ApplyBonusEffect(GameStatePlayingData& data, std::unique_ptr<BonusEffect> effect);
		friend void UpdateActiveBonusEffects(GameStatePlayingData& data, float timeDelta);
		friend void UpdateBonuses(GameStatePlayingData& data, float timeDelta);

	private:
		struct ActiveBonusEffect
		{
			std::unique_ptr<BonusEffect> effect;
			float remainingTime = 0.f;
		};

		sf::Font font;
		sf::Music music;
		sf::SoundBuffer bonusPickupSoundBuffer;
		sf::Sound bonusPickupSound;
		sf::SoundBuffer ballLostSoundBuffer;
		sf::Sound ballLostSound;

		sf::Text scoreText;
		sf::Text healthText;
		sf::Text inputHintText;
		MenuBackground levelBackground;
		bool hasLevelBackground = false;
		Paddle paddle;
		Ball ball;
		BlocksDestroyObserver blocksObserver;
		std::vector<std::unique_ptr<Block>> blocks;
		std::vector<std::unique_ptr<Bonus>> bonuses;
		std::vector<ActiveBonusEffect> activeBonusEffects;
		BonusFactory bonusFactory;

		int score = 0;
		int health = 0;
		
		//Blocks creator
		std::unordered_map<BlockType, std::unique_ptr<BlockFactory>> factories;
		
		//Levels
		LevelLoader levelLoader;
		int currentLevel = 0;
	};

	void InitGameStatePlaying(GameStatePlayingData& data);
	void ShutdownGameStatePlaying(GameStatePlayingData& data);
	void HandleGameStatePlayingWindowEvent(GameStatePlayingData& data, const sf::Event& event);
	void UpdateGameStatePlaying(GameStatePlayingData& data, float timeDelta);
	void DrawGameStatePlaying(GameStatePlayingData& data, sf::RenderWindow& window);
}
