#include "GameStatePlaying.h"

#include "Application.h"
#include "Game.h"
#include "Text.h"
#include "Ball.h"
#include "Block.h"
#include "BlockFactory.h"

#include <assert.h>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>


namespace Arkanoid
{
	void ClearActiveBonusEffects(GameStatePlayingData& data)
	{
		for (auto& activeEffect : data.activeBonusEffects)
		{
			activeEffect.effect->Revert(data.paddle, data.ball, data.health);
		}

		data.activeBonusEffects.clear();
		data.bonuses.clear();
		data.paddle.ResetModifiers();
		data.ball.ResetModifiers();
	}

	void ApplyBonusEffect(GameStatePlayingData& data, std::unique_ptr<BonusEffect> effect)
	{
		const std::string effectName = effect->GetName();

		for (auto it = data.activeBonusEffects.begin(); it != data.activeBonusEffects.end();)
		{
			if (it->effect->GetName() == effectName)
			{
				it->effect->Revert(data.paddle, data.ball, data.health);
				it = data.activeBonusEffects.erase(it);
			}
			else
			{
				++it;
			}
		}

		effect->Apply(data.paddle, data.ball, data.health);

		if (effect->GetDuration() <= 0.f)
		{
			return;
		}

		GameStatePlayingData::ActiveBonusEffect activeEffect;
		activeEffect.remainingTime = effect->GetDuration();
		activeEffect.effect = std::move(effect);
		data.activeBonusEffects.push_back(std::move(activeEffect));
	}

	void UpdateActiveBonusEffects(GameStatePlayingData& data, float timeDelta)
	{
		for (auto it = data.activeBonusEffects.begin(); it != data.activeBonusEffects.end();)
		{
			it->remainingTime -= timeDelta;
			if (it->remainingTime <= 0.f)
			{
				it->effect->Revert(data.paddle, data.ball, data.health);
				it = data.activeBonusEffects.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void UpdateBonuses(GameStatePlayingData& data, float timeDelta)
	{
		for (auto& bonus : data.bonuses)
		{
			bonus->Update(timeDelta);
		}

		for (auto it = data.bonuses.begin(); it != data.bonuses.end();)
		{
			if ((*it)->IsCollected(data.paddle))
			{
				data.bonusPickupSound.play();
				ApplyBonusEffect(data, (*it)->TakeEffect());
				data.healthText.setString("Health: " + std::to_string(data.health));
				it = data.bonuses.erase(it);
			}
			else if ((*it)->IsMissed(static_cast<float>(SETTINGS.SCREEN_HEIGHT)))
			{
				it = data.bonuses.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void SaveGameState(GameStatePlayingData& data)
	{
		std::ofstream file(SETTINGS.SAVE_FILE_PATH);
		if (!file.is_open())
		{
			return;
		}

		const sf::Vector2f paddlePosition = data.paddle.GetPosition();
		const sf::Vector2f ballPosition = data.ball.GetPosition();
		const sf::Vector2f ballVelocity = data.ball.GetVelocity();

		file << "level " << data.currentLevel << '\n';
		file << "score " << data.score << '\n';
		file << "health " << data.health << '\n';
		file << "paddle " << paddlePosition.x << ' ' << paddlePosition.y << '\n';
		file << "ball " << ballPosition.x << ' ' << ballPosition.y << ' ' << ballVelocity.x << ' ' << ballVelocity.y << '\n';

		file << "destroyed";
		for (size_t i = 0; i < data.blocks.size(); ++i)
		{
			if (data.blocks[i]->IsDestroyed())
			{
				file << ' ' << i;
			}
		}
		file << '\n';
	}

	bool LoadSavedGameState(GameStatePlayingData& data)
	{
		std::ifstream file(SETTINGS.SAVE_FILE_PATH);
		if (!file.is_open())
		{
			return false;
		}

		int loadedLevel = 0;
		int loadedScore = 0;
		int loadedHealth = SETTINGS.INITIAL_HEALTH;
		sf::Vector2f paddlePosition;
		sf::Vector2f ballPosition;
		sf::Vector2f ballVelocity;
		std::vector<size_t> destroyedBlocks;
		std::string label;

		if (!(file >> label >> loadedLevel) || label != "level")
		{
			return false;
		}
		if (!(file >> label >> loadedScore) || label != "score")
		{
			return false;
		}
		if (!(file >> label))
		{
			return false;
		}
		if (label == "health")
		{
			if (!(file >> loadedHealth))
			{
				return false;
			}
			if (!(file >> label))
			{
				return false;
			}
		}
		if (label != "paddle" || !(file >> paddlePosition.x >> paddlePosition.y))
		{
			return false;
		}
		if (!(file >> label >> ballPosition.x >> ballPosition.y >> ballVelocity.x >> ballVelocity.y) || label != "ball")
		{
			return false;
		}
		if (!(file >> label) || label != "destroyed")
		{
			return false;
		}

		std::string destroyedLine;
		std::getline(file, destroyedLine);
		std::stringstream destroyedStream(destroyedLine);
		size_t blockIndex = 0;
		while (destroyedStream >> blockIndex)
		{
			destroyedBlocks.push_back(blockIndex);
		}

		if (loadedLevel < 0 || loadedLevel >= data.levelLoader.GetLevelsCount())
		{
			return false;
		}

		ClearActiveBonusEffects(data);
		LoadGameLevel(data, loadedLevel);

		data.score = loadedScore;
		data.health = loadedHealth;
		data.paddle.SetPosition(paddlePosition);
		data.ball.SetPosition(ballPosition);
		data.ball.SetVelocity(ballVelocity);

		for (size_t index : destroyedBlocks)
		{
			if (index < data.blocks.size() && !data.blocks[index]->IsDestroyed())
			{
				data.blocks[index]->Destroy();
			}
		}

		return true;
	}

	void InitBlockFactories(GameStatePlayingData& data)
	{
		data.factories.clear();

		data.factories.emplace(BlockType::Regular, std::make_unique<RegularBlockFactory>());
		data.factories.emplace(BlockType::ThreeHit, std::make_unique<ThreeHitBlockFactory>());
		data.factories.emplace(BlockType::Glass, std::make_unique<GlassBlockFactory>());
		data.factories.emplace(BlockType::Unbreakable, std::make_unique<UnbreakableBlockFactory>());
	}

	void LoadGameLevel(GameStatePlayingData& data, int levelIndex)
	{
		assert(levelIndex >= 0 && levelIndex < data.levelLoader.GetLevelsCount());

		ClearActiveBonusEffects(data);
		data.currentLevel = levelIndex;
		data.blocks.clear();
		data.blocksObserver.Reset();

		data.hasLevelBackground = levelIndex < static_cast<int>(SETTINGS.LEVEL_BACKGROUND_PATHS.size());
		if (data.hasLevelBackground)
		{
			InitMenuBackground(data.levelBackground, SETTINGS.LEVEL_BACKGROUND_PATHS[levelIndex], sf::Color(0, 0, 0, 70));
		}

		data.paddle.Init(static_cast<float>(SETTINGS.SCREEN_WIDTH), static_cast<float>(SETTINGS.SCREEN_HEIGHT));
		data.ball.Init(static_cast<float>(SETTINGS.SCREEN_WIDTH), static_cast<float>(SETTINGS.SCREEN_HEIGHT));

		const LevelData& level = data.levelLoader.GetLevel(levelIndex);

		if (level.columns <= 0 || level.rows <= 0)
		{
			return;
		}

		const float marginX = 40.f;
		const float startY = 90.f;
		const float spacingX = 5.f;
		const float spacingY = 8.f;

		const float blockWidth =
			(SETTINGS.SCREEN_WIDTH - marginX * 2.f - spacingX * (level.columns - 1)) / level.columns;

		const sf::Vector2f blockSize = { blockWidth, 25.f };

		for (const LevelBlockData& blockData : level.blocks)
		{
			const sf::Vector2f position =
			{
				marginX + blockData.gridPosition.x * (blockSize.x + spacingX),
				startY + blockData.gridPosition.y * (blockSize.y + spacingY)
			};

			BlockFactory& factory = *data.factories.at(blockData.blockType);
			std::unique_ptr<Block> block = factory.CreateBlock(position, blockSize);
			if (blockData.blockType != BlockType::Unbreakable)
			{
				data.blocksObserver.Observe(*block);
			}

			data.blocks.push_back(std::move(block));
		}
	}

	void InitGameStatePlaying(GameStatePlayingData& data)
	{
		data.score = 0;
		data.health = SETTINGS.INITIAL_HEALTH;
		InitBlockFactories(data);
		assert(data.levelLoader.LoadLevelsFromFile(SETTINGS.LEVELS_CONFIG_PATH));
		LoadGameLevel(data, 0);

		assert(data.font.loadFromFile(SETTINGS.FONTS_PATH + "Roboto-Regular.ttf"));
		assert(data.music.openFromFile(SETTINGS.BATTLE_MUSIC_PATH));
		data.music.setLoop(true);
		data.music.play();
		assert(data.bonusPickupSoundBuffer.loadFromFile(SETTINGS.BONUS_PICKUP_SOUND_PATH));
		data.bonusPickupSound.setBuffer(data.bonusPickupSoundBuffer);
		assert(data.ballLostSoundBuffer.loadFromFile(SETTINGS.BALL_LOST_SOUND_PATH));
		data.ballLostSound.setBuffer(data.ballLostSoundBuffer);

		data.scoreText.setFont(data.font);
		data.scoreText.setCharacterSize(24);
		data.scoreText.setFillColor(sf::Color::White);
		data.scoreText.setPosition(20.f, 20.f);
		data.scoreText.setString("Score: " + std::to_string(data.score));

		data.healthText.setFont(data.font);
		data.healthText.setCharacterSize(24);
		data.healthText.setFillColor(sf::Color::White);
		data.healthText.setString("Health: " + std::to_string(data.health));

		data.inputHintText.setFont(data.font);
		data.inputHintText.setCharacterSize(18);
		data.inputHintText.setFillColor(sf::Color(180, 180, 180));
		data.inputHintText.setPosition(20.f, 50.f);
		data.inputHintText.setString("F5: save  F9: load  Esc: pause");
	}

	void ShutdownGameStatePlaying(GameStatePlayingData& data)
	{
		data.music.stop();
		ClearActiveBonusEffects(data);
		data.blocks.clear();
	}

	void HandleGameStatePlayingWindowEvent(GameStatePlayingData& data, const sf::Event& event)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				Application::Instance().GetGame().PauseGame();
			}
			else if (event.key.code == sf::Keyboard::F5)
			{
				SaveGameState(data);
			}
			else if (event.key.code == sf::Keyboard::F9)
			{
				LoadSavedGameState(data);
			}
		}
	}

	void UpdateGameStatePlaying(GameStatePlayingData& data, float timeDelta)
	{
		data.paddle.Update(timeDelta);

		data.ball.Update(timeDelta);
		UpdateActiveBonusEffects(data, timeDelta);
		UpdateBonuses(data, timeDelta);

		data.ball.CheckCollision(data.paddle);

		if (data.ball.IsBelowField())
		{
			data.ballLostSound.play();
			--data.health;
			data.healthText.setString("Health: " + std::to_string(data.health));
			ClearActiveBonusEffects(data);

			if (data.health <= 0)
			{
				Game& game = Application::Instance().GetGame();
				game.UpdateRecord(SETTINGS.PLAYER_NAME, data.score);
				game.LoseGame();
			}
			else
			{
				data.ball.Restart();
			}

			return;
		}

		for (std::unique_ptr<Block>& block : data.blocks)
		{
			block->Update(timeDelta);

			if (block->IsDestroyed())
			{
				continue;
			}

			if (data.ball.CheckCollision(*block))
			{
				if (block->IsDestroyed())
				{
					data.score += block->GetScorePoints();

					if (data.bonusFactory.ShouldSpawnBonus())
					{
						const sf::FloatRect blockBounds = block->GetBounds();
						data.bonuses.push_back(data.bonusFactory.CreateRandomBonus({
							blockBounds.left + blockBounds.width * 0.5f,
							blockBounds.top + blockBounds.height * 0.5f
						}));
					}
				}

				break;
			}
		}

		if (data.blocksObserver.AreAllBlocksDestroyed())
		{
			const int nextLevel = data.currentLevel + 1;
			if (nextLevel < data.levelLoader.GetLevelsCount())
			{
				LoadGameLevel(data, nextLevel);
			}
			else
			{
				Game& game = Application::Instance().GetGame();
				game.UpdateRecord(SETTINGS.PLAYER_NAME, data.score);
				game.WinGame();
			}

			return;
		}

		data.scoreText.setString("Score: " + std::to_string(data.score));
		data.healthText.setString("Health: " + std::to_string(data.health));
	}

	void DrawGameStatePlaying(GameStatePlayingData& data, sf::RenderWindow& window)
	{
		if (data.hasLevelBackground)
		{
			DrawMenuBackground(data.levelBackground, window);
		}

		for (const std::unique_ptr<Block>& block : data.blocks)
		{
			block->Draw(window);
		}

		for (const std::unique_ptr<Bonus>& bonus : data.bonuses)
		{
			bonus->Draw(window);
		}

		data.paddle.Draw(window);
		data.ball.Draw(window);

		window.draw(data.scoreText);
		data.healthText.setOrigin(GetTextOrigin(data.healthText, { 0.5f, 0.f }));
		data.healthText.setPosition(window.getView().getSize().x / 2.f, 20.f);
		window.draw(data.healthText);
		window.draw(data.inputHintText);
	}
}
