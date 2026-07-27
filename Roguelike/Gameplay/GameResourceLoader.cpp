#include "pch.h"
#include "GameResourceLoader.h"

#include "GameConfig.h"
#include "ResourceSystem.h"

#include <string>

namespace Roguelike
{
namespace
{
void LoadTexture(const GameConfig::TextureConfig& texture)
{
    Engine::ResourceSystem::Instance()->LoadTexture(texture.key, texture.path, texture.smooth);
}

void LoadCharacterTextures(const GameConfig::CharacterTextureSet& textures)
{
    Engine::ResourceSystem::Instance()->LoadTexture(textures.baseKey, textures.basePath);
    Engine::ResourceSystem::Instance()->LoadTexture(textures.defaultKey, textures.defaultPath);
    Engine::ResourceSystem::Instance()->LoadTexture(textures.downKey, textures.downPath);
    Engine::ResourceSystem::Instance()->LoadTexture(textures.leftKey, textures.leftPath);
    Engine::ResourceSystem::Instance()->LoadTexture(textures.rightKey, textures.rightPath);
    Engine::ResourceSystem::Instance()->LoadTexture(textures.upKey, textures.upPath);
}

void LoadNumberedTileTextures(const std::string& keyPrefix,
                              const std::string& folder,
                              const std::string& filePrefix,
                              int count)
{
    for (int i = 1; i <= count; ++i)
    {
        const std::string index = std::to_string(i);
        Engine::ResourceSystem::Instance()->LoadTexture(
            keyPrefix + "_" + index,
            "Resources/Textures/" + folder + "/" + filePrefix + "_" + index + ".png");
    }
}

void LoadNumberedUITextures(const std::string& keyPrefix,
                            const std::string& filePrefix,
                            int count)
{
    for (int i = 1; i <= count; ++i)
    {
        const std::string index = std::to_string(i);
        Engine::ResourceSystem::Instance()->LoadTexture(
            keyPrefix + "_" + index, "Resources/UI/" + filePrefix + "_" + index + ".png", false);
    }
}
}

void GameResourceLoader::Load()
{
    for (const GameConfig::CharacterTextureSet& textures : GameConfig::CharacterTextures)
    {
        LoadCharacterTextures(textures);
    }

    // Tile variants are grouped by folder and loaded by predictable numbered names.
    LoadNumberedTileTextures("wall", "Walls", "Wall",
                             static_cast<int>(GameConfig::WallTextureKeys.size()));
    LoadNumberedTileTextures("floor", "Floors", "Floor",
                             static_cast<int>(GameConfig::FloorTextureKeys.size()));

    for (const GameConfig::TextureConfig& texture : GameConfig::UITextures)
    {
        LoadTexture(texture);
    }

    LoadNumberedUITextures("ui_start_fire", "fire", 6);
    LoadTexture(GameConfig::DeadLightBackgroundTexture);
    LoadTexture(GameConfig::NextLevelBackgroundTexture);

    for (const GameConfig::BackgroundTrackConfig& track : GameConfig::BackgroundTracks)
    {
        Engine::ResourceSystem::Instance()->LoadSoundBuffer(track.name, track.path);
    }
}
}  // namespace Roguelike
