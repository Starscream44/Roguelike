// @file ResourceSystem.cpp

#include "pch.h"
#include "ResourceSystem.h"

#include "Logger.h"

namespace Engine
{
namespace
{
template <typename ResourceMap>
void DeleteResourceMap(ResourceMap& resources)
{
    for (auto& resourcePair : resources)
    {
        delete resourcePair.second;
    }

    resources.clear();
}
}  // namespace

ResourceSystem* ResourceSystem::Instance()
{
    static ResourceSystem resourceSystem;
    return &resourceSystem;
}

void ResourceSystem::LoadTexture(const std::string& name, std::string sourcePath, bool isSmooth)
{
    if (textures.find(name) != textures.end())
    {
        return;
    }

    sf::Texture* newTexture = new sf::Texture();

    if (newTexture->loadFromFile(sourcePath))
    {
        newTexture->setSmooth(isSmooth);
        textures.emplace(name, newTexture);
    }
    else
    {
        LOG_ERROR("Texture load failed: " + name + " from " + sourcePath);
        delete newTexture;
    }
}

bool ResourceSystem::HasTexture(const std::string& name) const
{
    return textures.find(name) != textures.end();
}

const sf::Texture* ResourceSystem::GetTextureShared(const std::string& name) const
{
    auto texture = textures.find(name);

    if (texture == textures.end())
    {
        LOG_ERROR("Texture not found: " + name);
        return nullptr;
    }

    return texture->second;
}

sf::Texture* ResourceSystem::GetTextureCopy(const std::string& name) const
{
    return new sf::Texture(*textures.find(name)->second);
}

void ResourceSystem::DeleteSharedTexture(const std::string& name)
{
    auto texturePair = textures.find(name);

    if (texturePair == textures.end())
    {
        return;
    }

    sf::Texture* deletingTexture = texturePair->second;
    textures.erase(texturePair);
    delete deletingTexture;
}

void ResourceSystem::LoadTextureMap(const std::string& name, std::string sourcePath,
                                    sf::Vector2u elementPixelSize, int totalElements, bool isSmooth)
{
    if (textureMaps.find(name) != textureMaps.end())
    {
        return;
    }

    sf::Texture textureMap;

    if (textureMap.loadFromFile(sourcePath))
    {
        std::vector<sf::Texture*> textureMapElements;

        auto textureSize = textureMap.getSize();
        int loadedElements = 0;

        for (int y = 0; y <= static_cast<int>(textureSize.y - elementPixelSize.y);
             y += elementPixelSize.y)
        {
            if (loadedElements == totalElements)
            {
                break;
            }

            for (int x = 0; x <= static_cast<int>(textureSize.x - elementPixelSize.x);
                 x += elementPixelSize.x)
            {
                if (loadedElements == totalElements)
                {
                    break;
                }

                sf::Texture* newTextureMapElement = new sf::Texture();

                if (newTextureMapElement->loadFromFile(
                        sourcePath, sf::IntRect(x, y, elementPixelSize.x, elementPixelSize.y)))
                {
                    newTextureMapElement->setSmooth(isSmooth);
                    textureMapElements.push_back(newTextureMapElement);
                }
                else
                {
                    delete newTextureMapElement;
                }

                loadedElements++;
            }
        }

        textureMaps.emplace(name, textureMapElements);
    }
}

const sf::Texture* ResourceSystem::GetTextureMapElementShared(const std::string& name,
                                                              int elementIndex) const
{
    return textureMaps.find(name)->second[elementIndex];
}

sf::Texture* ResourceSystem::GetTextureMapElementCopy(const std::string& name,
                                                      int elementIndex) const
{
    return new sf::Texture(*textureMaps.find(name)->second[elementIndex]);
}

int ResourceSystem::GetTextureMapElementsCount(const std::string& name) const
{
    return static_cast<int>(textureMaps.find(name)->second.size());
}

void ResourceSystem::DeleteSharedTextureMap(const std::string& name)
{
    auto textureMap = textureMaps.find(name);

    if (textureMap == textureMaps.end())
    {
        return;
    }

    for (int i = 0; i < textureMap->second.size(); i++)
    {
        delete textureMap->second[i];
    }

    textureMaps.erase(textureMap);
}

void ResourceSystem::LoadSoundBuffer(const std::string& name, std::string sourcePath)
{
    if (soundBuffers.find(name) != soundBuffers.end())
    {
        return;
    }

    sf::SoundBuffer* newSoundBuffer = new sf::SoundBuffer();

    if (newSoundBuffer->loadFromFile(sourcePath))
    {
        soundBuffers.emplace(name, newSoundBuffer);
    }
    else
    {
        delete newSoundBuffer;
    }
}

bool ResourceSystem::HasSoundBuffer(const std::string& name) const
{
    return soundBuffers.find(name) != soundBuffers.end();
}

const sf::SoundBuffer* ResourceSystem::GetSoundBufferShared(const std::string& name) const
{
    return soundBuffers.find(name)->second;
}

void ResourceSystem::DeleteSharedSoundBuffer(const std::string& name)
{
    auto soundBufferPair = soundBuffers.find(name);

    if (soundBufferPair == soundBuffers.end())
    {
        return;
    }

    sf::SoundBuffer* deletingSoundBuffer = soundBufferPair->second;
    soundBuffers.erase(soundBufferPair);
    delete deletingSoundBuffer;
}

void ResourceSystem::Clear()
{
    DeleteAllTextures();
    DeleteAllTextureMaps();
    DeleteAllSoundBuffers();
}

void ResourceSystem::DeleteAllTextures()
{
    DeleteResourceMap(textures);
}

void ResourceSystem::DeleteAllTextureMaps()
{
    for (auto& textureMapPair : textureMaps)
    {
        for (sf::Texture* texture : textureMapPair.second)
        {
            delete texture;
        }
    }

    textureMaps.clear();
}

void ResourceSystem::DeleteAllSoundBuffers()
{
    DeleteResourceMap(soundBuffers);
}
}  // namespace Engine
