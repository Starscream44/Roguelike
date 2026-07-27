#include "pch.h"
#include "DeveloperLevel.h"

#include "AudioComponent.h"
#include "EnemySpawner.h"
#include "GameConfig.h"
#include "GameObject.h"
#include "GameResourceLoader.h"
#include "GameUIComponent.h"
#include "GameWorld.h"
#include "ItemSpawner.h"
#include "LevelProgress.h"
#include "Logger.h"
#include "MazeGenerator.h"
#include "ResourceSystem.h"
#include "TransformComponent.h"

#include <cstdlib>
#include <vector>

namespace Roguelike
{
void DeveloperLevel::Start()
{
    Engine::GameWorld::Instance()->SetPaused(true);

    LOG_INFO("DeveloperLevel start.");

    LoadResources();
    GenerateMaze();
    CreatePlayer();
    SpawnItems();
    SpawnEnemies();
    RegisterPlayerTargets();
    RegisterProjectileObstacles();
    CreateMusic();
    CreateUI();

    LOG_INFO("DeveloperLevel created successfully.");
}

void DeveloperLevel::Restart()
{
    LOG_WARN("DeveloperLevel restart.");

    Stop();
    Start();
}

void DeveloperLevel::Stop()
{
    LOG_INFO("DeveloperLevel stop.");

    Engine::GameWorld::Instance()->Clear();

    player = nullptr;
    playerObject = nullptr;

    enemies.clear();
    floors.clear();
    walls.clear();
    floorPositions.clear();
}

void DeveloperLevel::LoadResources() { GameResourceLoader::Load(); }

void DeveloperLevel::GenerateMaze()
{
    MazeGenerator mazeGenerator(GameConfig::MazeWidth, GameConfig::MazeHeight, this);

    mazeGenerator.Generate();

    floorPositions = mazeGenerator.GetFloorPositions();
}

void DeveloperLevel::CreatePlayer()
{
    player = std::make_shared<Player>();
    playerObject = player->GetGameObject();

    float startX = GameConfig::PlayerStartTileX * GameConfig::TileSize;
    float startY = GameConfig::PlayerStartTileY * GameConfig::TileSize;

    playerObject->GetComponent<Engine::TransformComponent>()->SetWorldPosition(startX, startY);
}

void DeveloperLevel::SpawnEnemies()
{
    EnemySpawner enemySpawner;
    int extraEnemies = LevelProgress::GetLevel() - 1;

    EnemySpawnSettings creeperSettings;
    creeperSettings.count = GameConfig::CreeperSpawnCount + extraEnemies / 2;
    creeperSettings.minDistanceFromPlayer = GameConfig::EnemyMinSpawnDistanceFromPlayer;
    creeperSettings.enemyType = EnemyType::Creeper;

    std::vector<std::unique_ptr<Character>> spawnedCreepers =
        enemySpawner.Spawn(creeperSettings, floorPositions, playerObject);

    EnemySpawnSettings warriorSettings;
    warriorSettings.count = GameConfig::WarriorSpawnCount + extraEnemies;
    warriorSettings.minDistanceFromPlayer = GameConfig::EnemyMinSpawnDistanceFromPlayer;
    warriorSettings.enemyType = EnemyType::Warrior;

    std::vector<std::unique_ptr<Character>> spawnedWarriors =
        enemySpawner.Spawn(warriorSettings, floorPositions, playerObject);

    for (auto& enemy : spawnedCreepers)
    {
        enemies.push_back(std::move(enemy));
    }

    for (auto& enemy : spawnedWarriors)
    {
        enemies.push_back(std::move(enemy));
    }
}

void DeveloperLevel::SpawnItems()
{
    ItemSpawner itemSpawner;
    itemSpawner.Spawn(floorPositions, playerObject);
}

void DeveloperLevel::RegisterPlayerTargets()
{
    if (player == nullptr)
    {
        return;
    }

    for (const auto& enemy : enemies)
    {
        player->AddAttackTarget(enemy->GetGameObject());
    }
}

void DeveloperLevel::RegisterProjectileObstacles()
{
    if (player == nullptr)
    {
        return;
    }

    std::vector<Engine::GameObject*> wallObjects;

    for (const auto& wall : walls)
    {
        wallObjects.push_back(wall->GetGameObject());
    }

    player->SetObstacles(wallObjects);
}

void DeveloperLevel::CreateMusic()
{
    const char* trackName = ChooseBackgroundTrack();

    if (trackName == nullptr)
    {
        LOG_WARN("No background music loaded.");
        return;
    }

    Engine::GameObject* musicObject = Engine::GameWorld::Instance()->CreateGameObject("Music");

    Engine::AudioComponent* music = musicObject->AddComponent<Engine::AudioComponent>();

    music->SetAudio(*Engine::ResourceSystem::Instance()->GetSoundBufferShared(trackName));

    music->SetLoop(true);
    music->SetVolume(GameConfig::MusicVolume);
    music->Play();
}

const char* DeveloperLevel::ChooseBackgroundTrack() const
{
    static int lastTrackIndex = -1;
    std::vector<int> loadedTrackIndexes;

    for (int i = 0; i < static_cast<int>(GameConfig::BackgroundTracks.size()); ++i)
    {
        const GameConfig::BackgroundTrackConfig& track = GameConfig::BackgroundTracks[i];

        if (Engine::ResourceSystem::Instance()->HasSoundBuffer(track.name))
        {
            loadedTrackIndexes.push_back(i);
        }
    }

    if (loadedTrackIndexes.empty())
    {
        return nullptr;
    }

    int chosenIndex = loadedTrackIndexes[std::rand() % loadedTrackIndexes.size()];

    if (loadedTrackIndexes.size() > 1)
    {
        while (chosenIndex == lastTrackIndex)
        {
            chosenIndex = loadedTrackIndexes[std::rand() % loadedTrackIndexes.size()];
        }
    }

    lastTrackIndex = chosenIndex;
    return GameConfig::BackgroundTracks[chosenIndex].name;
}

void DeveloperLevel::CreateUI()
{
    Engine::GameObject* uiObject = Engine::GameWorld::Instance()->CreateGameObject("UI");

    GameUIComponent* gameUI = uiObject->AddComponent<GameUIComponent>();

    gameUI->SetPlayer(playerObject);

    std::vector<Engine::GameObject*> enemyObjects;
    for (const auto& enemy : enemies)
    {
        enemyObjects.push_back(enemy->GetGameObject());
    }

    gameUI->SetLevelObjective(enemyObjects, LevelProgress::GetLevel());

    Engine::GameWorld::Instance()->AddPauseIgnoredGameObject(uiObject);
}
}  // namespace Roguelike
