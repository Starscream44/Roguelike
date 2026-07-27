#include "pch.h"
#include "Floor.h"

#include "GameConfig.h"
#include "GameWorld.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "SpriteRendererComponent.h"
#include "ResourceSystem.h"

namespace Roguelike
{
Floor::Floor(float x, float y, const std::string& textureKey)
{
    gameObject = Engine::GameWorld::Instance()->CreateGameObject("Floor");

    Engine::TransformComponent* transform = gameObject->GetComponent<Engine::TransformComponent>();

    transform->SetWorldPosition(x, y);

    Engine::SpriteRendererComponent* renderer =
        gameObject->AddComponent<Engine::SpriteRendererComponent>();

    Engine::ResourceSystem* resources = Engine::ResourceSystem::Instance();

    // Fallback protects level generation when a future tile variant is not loaded yet.
    const std::string safeTextureKey = resources->HasTexture(textureKey) ? textureKey : "floor_1";

    const sf::Texture* texture = resources->GetTextureShared(safeTextureKey);

    if (texture == nullptr)
    {
        return;
    }

    renderer->SetTexture(*texture);

    renderer->SetPixelSize(GameConfig::TilePixelSize, GameConfig::TilePixelSize);
}
}  // namespace Roguelike
