#include "pch.h"
#include "Wall.h"

#include "BoxColliderComponent.h"
#include "GameConfig.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Logger.h"
#include "ResourceSystem.h"
#include "SpriteRendererComponent.h"
#include "TransformComponent.h"

namespace Roguelike
{
Wall::Wall(float x, float y, const std::string& textureKey)
{
    gameObject = Engine::GameWorld::Instance()->CreateGameObject("Wall");

    auto transform = gameObject->GetComponent<Engine::TransformComponent>();

    transform->SetWorldPosition(x, y);

    auto renderer = gameObject->AddComponent<Engine::SpriteRendererComponent>();

    Engine::ResourceSystem* resources = Engine::ResourceSystem::Instance();

    // Fallback protects collision tiles from missing future wall variants.
    const std::string safeTextureKey = resources->HasTexture(textureKey) ? textureKey : "wall_1";

    const sf::Texture* texture = resources->GetTextureShared(safeTextureKey);

    if (texture != nullptr)
    {
        renderer->SetTexture(*texture);
        renderer->SetPixelSize(GameConfig::TilePixelSize, GameConfig::TilePixelSize);
    }

    if (texture == nullptr)
    {
        LOG_ERROR("Wall texture missing. Collision still created for tile.");
    }

    auto collider = gameObject->AddComponent<Engine::BoxColliderComponent>();
    collider->SetSize(GameConfig::TileSize, GameConfig::TileSize);
}

Engine::GameObject* Wall::GetGameObject() const { return gameObject; }
}  // namespace Roguelike
