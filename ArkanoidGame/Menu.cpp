#include "Menu.h"
#include <assert.h>
#include <algorithm>
#include <iterator>

namespace Arkanoid
{
	void MenuItem::SetTextString(const sf::String& value)
	{
		text.setString(value);
	}

	void MenuItem::SetTextFont(const sf::Font& font)
	{
		text.setFont(font);
	}

	void MenuItem::SetTextCharacterSize(unsigned int size)
	{
		text.setCharacterSize(size);
	}

	void MenuItem::SetTextFillColor(const sf::Color& color)
	{
		text.setFillColor(color);
	}

	void MenuItem::SetHintString(const sf::String& value)
	{
		hintText.setString(value);
	}

	void MenuItem::SetHintFont(const sf::Font& font)
	{
		hintText.setFont(font);
	}

	void MenuItem::SetHintCharacterSize(unsigned int size)
	{
		hintText.setCharacterSize(size);
	}

	void MenuItem::SetHintFillColor(const sf::Color& color)
	{
		hintText.setFillColor(color);
	}

	void MenuItem::SetChildrenLayout(Orientation orientation, Alignment alignment, float spacing)
	{
		childrenOrientation = orientation;
		childrenAlignment = alignment;
		childrenSpacing = spacing;
	}

	void MenuItem::AddChild(const MenuItem& child)
	{
		children.push_back(child);
	}

	void MenuItem::SetEnabled(bool enabled)
	{
		isEnabled = enabled;
	}

	bool MenuItem::IsEnabled() const
	{
		return isEnabled;
	}

	void MenuItem::SetOnPressCallback(const OnPressCallback& callback)
	{
		onPressCallback = callback;
	}

	void MenuItem::SetSelectionColors(const sf::Color& newSelectedColor, const sf::Color& newDeselectedColor)
	{
		selectedColor = newSelectedColor;
		deselectedColor = newDeselectedColor;
	}

	Orientation MenuItem::GetChildrenOrientation() const
	{
		return childrenOrientation;
	}

	Alignment MenuItem::GetChildrenAlignment() const
	{
		return childrenAlignment;
	}

	float MenuItem::GetChildrenSpacing() const
	{
		return childrenSpacing;
	}

	const sf::Text& MenuItem::GetText() const
	{
		return text;
	}

	const sf::Text& MenuItem::GetHintText() const
	{
		return hintText;
	}

	void Menu::Init(const MenuItem& item)
	{
		rootItem = item;

		InitMenuItem(rootItem);
		if (!rootItem.children.empty())
		{
			SelectMenuItem(rootItem.children.front());
		}
	}

	void Menu::InitMenuItem(MenuItem& item)
	{
		for (auto& child : item.children)
		{
			child.parent = &item;
			InitMenuItem(child);
		}
	}

	void Menu::Update(float)
	{
		// Currently menu does not need time-based updates.
	}

	void Menu::Draw(sf::RenderWindow& window, sf::Vector2f position, sf::Vector2f origin)
	{
		MenuItem& expandedItem = GetCurrentContext();

		std::vector<sf::Text*> texts;
		texts.reserve(expandedItem.children.size());

		for (auto& child : expandedItem.children)
		{
			if (child.isEnabled)
			{
				texts.push_back(&child.text);
			}
		}

		DrawTextList(
			window,
			texts,
			expandedItem.childrenSpacing,
			expandedItem.childrenOrientation,
			expandedItem.childrenAlignment,
			position,
			origin);
	}

	void Menu::PressOnSelectedItem()
	{
		if (!selectedItem)
		{
			return;
		}

		if (selectedItem->onPressCallback)
		{
			selectedItem->onPressCallback(*selectedItem);
			return;
		}

		// default behaviour
		if (!selectedItem->children.empty())
		{
			SelectMenuItem(selectedItem->children.front());
		}
	}

	void Menu::GoBack()
	{
		MenuItem& parent = GetCurrentContext();
		if (&parent != &rootItem)
		{
			SelectMenuItem(parent);
		}
	}

	void Menu::SwitchToPreviousMenuItem()
	{
		if (!selectedItem)
		{
			return;
		}

		MenuItem* parent = selectedItem->parent;
		assert(parent);

		auto it = std::find_if(parent->children.begin(), parent->children.end(), [this](const auto& item) {
			return selectedItem == &item;
			});

		if (it != parent->children.begin())
		{
			SelectMenuItem(*std::prev(it));
		}
	}

	void Menu::SwitchToNextMenuItem()
	{
		if (!selectedItem)
		{
			return;
		}

		MenuItem* parent = selectedItem->parent;
		assert(parent);

		auto it = std::find_if(parent->children.begin(), parent->children.end(), [this](const auto& item) {
			return selectedItem == &item;
			});

		it = std::next(it);
		if (it != parent->children.end())
		{
			SelectMenuItem(*it);
		}
	}

	void Menu::SelectMenuItem(MenuItem& item)
	{
		assert(&item != &rootItem);

		if (selectedItem == &item)
		{
			return;
		}

		if (!item.isEnabled)
		{
			return;
		}

		if (selectedItem)
		{
			selectedItem->text.setFillColor(selectedItem->deselectedColor);
		}

		selectedItem = &item;

		if (selectedItem)
		{
			selectedItem->text.setFillColor(selectedItem->selectedColor);
		}
	}

	MenuItem& Menu::GetCurrentContext()
	{
		return selectedItem ? *(selectedItem->parent) : rootItem;
	}

	const MenuItem& Menu::GetCurrentContext() const
	{
		return selectedItem ? *(selectedItem->parent) : rootItem;
	}
}