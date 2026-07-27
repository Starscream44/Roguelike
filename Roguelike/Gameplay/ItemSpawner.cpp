#include "pch.h"
#include "ItemSpawner.h"

#include "GameConfig.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "ItemPickupComponent.h"
#include "Logger.h"
#include "TransformComponent.h"
#include "ItemData.h"

#include <algorithm>
#include <cstdlib>

namespace Roguelike
{
void ItemSpawner::Spawn(const std::vector<Engine::Vector2Df>& floorPositions,
                        Engine::GameObject* player)
{
    if (player == nullptr)
    {
        LOG_ERROR("ItemSpawner failed. Player is null.");
        return;
    }

    if (floorPositions.empty())
    {
        LOG_ERROR("ItemSpawner failed. No floor positions.");
        return;
    }

    int spawnedCount = 0;
    int attempts = 0;
    int maxAttempts = GameConfig::ItemSpawnCount * GameConfig::SpawnMaxAttemptsMultiplier;
    std::vector<int> usedPositionIndexes;

    // Items use the same attempt budget as enemies, but only need distance and duplicate checks.
    while (spawnedCount < GameConfig::ItemSpawnCount && attempts < maxAttempts)
    {
        attempts++;

        int positionIndex = std::rand() % static_cast<int>(floorPositions.size());

        if (std::find(usedPositionIndexes.begin(), usedPositionIndexes.end(), positionIndex) !=
            usedPositionIndexes.end())
        {
            continue;
        }

        const Engine::Vector2Df& position = floorPositions[positionIndex];

        if (!IsPositionFarEnoughFromPlayer(position, player,
                                           GameConfig::ItemMinSpawnDistanceFromPlayer))
        {
            continue;
        }

        usedPositionIndexes.push_back(positionIndex);

        // Item definitions are data-driven in GameConfig::Items.
        const ItemData& itemData = GameConfig::Items[std::rand() % GameConfig::Items.size()];

        Engine::GameObject* itemObject =
            Engine::GameWorld::Instance()->CreateGameObject(itemData.name);

        Engine::TransformComponent* transform =
            itemObject->GetComponent<Engine::TransformComponent>();
        transform->SetWorldPosition(position.x, position.y);

        ItemStack item;
        item.data = &itemData;
        item.count = itemData.defaultCount;

        ItemPickupComponent* pickup = itemObject->AddComponent<ItemPickupComponent>();
        pickup->SetItem(item);

        spawnedCount++;
    }

    LOG_INFO("ItemSpawner spawned items: " + std::to_string(spawnedCount));
}

bool ItemSpawner::IsPositionFarEnoughFromPlayer(const Engine::Vector2Df& position,
                                                Engine::GameObject* player,
                                                float minDistance) const
{
    Engine::TransformComponent* playerTransform =
        player->GetComponent<Engine::TransformComponent>();

    if (playerTransform == nullptr)
    {
        LOG_ERROR("ItemSpawner failed. Player has no TransformComponent.");
        return false;
    }

    Engine::Vector2Df playerPosition = playerTransform->GetWorldPosition();
    Engine::Vector2Df direction = {position.x - playerPosition.x, position.y - playerPosition.y};

    return direction.GetLength() >= minDistance;
}
}  // namespace Roguelike
