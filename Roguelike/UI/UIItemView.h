#pragma once

#include "ItemData.h"

#include <SFML/Graphics.hpp>

#include <string>

namespace Roguelike
{
struct UIItemView
{
    ItemStack stack;
    sf::Color iconColor = sf::Color::White;

    static UIItemView FromStack(const ItemStack& itemStack)
    {
        UIItemView view;
        view.stack = itemStack;
        view.iconColor = itemStack.GetIconColor();
        return view;
    }
};

inline bool CanUseOnHotbar(const UIItemView& item) { return CanUseOnHotbar(item.stack); }

inline bool CanEquip(const UIItemView& item) { return CanEquip(item.stack); }

inline std::string GetItemTextureKey(const ItemStack& item)
{
    if (item.data == nullptr || item.data->textureKey == nullptr)
    {
        return "";
    }

    return item.data->textureKey;
}
}  // namespace Roguelike
