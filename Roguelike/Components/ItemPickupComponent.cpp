#include "pch.h"
#include "ItemPickupComponent.h"

#include "GameConfig.h"
#include "GameNotifications.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "InventoryComponent.h"
#include "RenderSystem.h"
#include "ResourceSystem.h"
#include "TransformComponent.h"
#include "UIItemView.h"

namespace Roguelike
{
namespace
{
// World item visual tuning. Gameplay radius and base icon size live in GameConfig.
const float ItemTextureScale = 1.75f;
const float ItemShadowWidthScale = 1.55f;
const float ItemShadowHeightScale = 0.42f;
const float ItemShadowYOffsetScale = 0.6f;
const float ItemFallbackRotation = 45.0f;

bool RenderItemTexture(const ItemStack& item, sf::Vector2f center, float size)
{
    const std::string textureKey = GetItemTextureKey(item);

    if (textureKey.empty() || !Engine::ResourceSystem::Instance()->HasTexture(textureKey))
    {
        return false;
    }

    const sf::Texture* texture = Engine::ResourceSystem::Instance()->GetTextureShared(textureKey);
    sf::Vector2u textureSize = texture->getSize();

    if (textureSize.x == 0 || textureSize.y == 0)
    {
        return false;
    }

    sf::Sprite sprite;
    sprite.setTexture(*texture);
    sprite.setOrigin({static_cast<float>(textureSize.x) * 0.5f,
                      static_cast<float>(textureSize.y) * 0.5f});
    sprite.setPosition(center);
    sprite.setScale(size / static_cast<float>(textureSize.x),
                    size / static_cast<float>(textureSize.y));

    Engine::RenderSystem::Instance()->Render(sprite);
    return true;
}
}  // namespace

ItemPickupComponent::ItemPickupComponent(Engine::GameObject* gameObject)
    : PickupComponent(gameObject)
{
    transform = gameObject->GetComponent<Engine::TransformComponent>();
}

void ItemPickupComponent::SetItem(const ItemStack& newItem) { item = newItem; }

void ItemPickupComponent::Update(float deltaTime)
{
    if (transform == nullptr)
    {
        return;
    }

    Engine::Vector2Df position = transform->GetWorldPosition();
    float radius = GameConfig::ItemPickupRadius;

    // Pickup bounds are centered on the item and read by PickupComponent trigger checks.
    bounds = sf::FloatRect(position.x - radius, position.y - radius, radius * 2.0f,
                           radius * 2.0f);
}

void ItemPickupComponent::Render()
{
    if (transform == nullptr || IsCollected())
    {
        return;
    }

    Engine::Vector2Df position = transform->GetWorldPosition();
    float size = GameConfig::ItemIconSize;

    sf::RectangleShape shadow;
    shadow.setSize({size * ItemShadowWidthScale, size * ItemShadowHeightScale});
    shadow.setOrigin({size * ItemShadowWidthScale * 0.5f,
                      size * ItemShadowHeightScale * 0.5f});
    shadow.setPosition({position.x, position.y + size * ItemShadowYOffsetScale});
    shadow.setFillColor(sf::Color(0, 0, 0, 70));

    Engine::RenderSystem::Instance()->Render(shadow);

    if (!RenderItemTexture(item, {position.x, position.y}, size * ItemTextureScale))
    {
        sf::RectangleShape icon;
        icon.setSize({size, size});
        icon.setOrigin({size / 2.0f, size / 2.0f});
        icon.setPosition({position.x, position.y});
        icon.setRotation(ItemFallbackRotation);
        icon.setFillColor(item.GetIconColor());
        icon.setOutlineColor(sf::Color(245, 225, 170, 220));
        icon.setOutlineThickness(2.0f);

        Engine::RenderSystem::Instance()->Render(icon);
    }
}

bool ItemPickupComponent::Collect(Engine::GameObject* collectorObject)
{
    InventoryComponent* inventory = collectorObject->GetComponent<InventoryComponent>();

    if (inventory == nullptr)
    {
        return false;
    }

    if (!inventory->AddItem(item))
    {
        GameNotifications::Push("Inventory is full");
        return false;
    }

    Engine::GameWorld::Instance()->DestroyGameObject(gameObject);
    return true;
}
}  // namespace Roguelike
