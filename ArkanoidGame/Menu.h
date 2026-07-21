#pragma once
#include "SFML/Graphics.hpp"
#include "Text.h"
#include <functional>
#include <vector>

namespace Arkanoid
{
	class Menu;

	class MenuItem
	{
		friend class Menu;

	public:
		using OnPressCallback = std::function<void(MenuItem& item)>;

		void SetTextString(const sf::String& value);
		void SetTextFont(const sf::Font& font);
		void SetTextCharacterSize(unsigned int size);
		void SetTextFillColor(const sf::Color& color);

		void SetHintString(const sf::String& value);
		void SetHintFont(const sf::Font& font);
		void SetHintCharacterSize(unsigned int size);
		void SetHintFillColor(const sf::Color& color);

		void SetChildrenLayout(Orientation orientation, Alignment alignment, float spacing = 0.f);

		void AddChild(const MenuItem& child);

		void SetEnabled(bool enabled);
		bool IsEnabled() const;

		void SetOnPressCallback(const OnPressCallback& callback);
		void SetSelectionColors(const sf::Color& selectedColor, const sf::Color& deselectedColor);

		Orientation GetChildrenOrientation() const;
		Alignment GetChildrenAlignment() const;
		float GetChildrenSpacing() const;

		const sf::Text& GetText() const;
		const sf::Text& GetHintText() const;

	private:
		sf::Text text;
		sf::Text hintText; // Visible when child item is selected

		Orientation childrenOrientation = Orientation::Vertical;
		Alignment childrenAlignment = Alignment::Min;
		float childrenSpacing = 0.f;

		sf::Color selectedColor = sf::Color::Yellow;
		sf::Color deselectedColor = sf::Color::White;

		bool isEnabled = true;
		std::vector<MenuItem> children;

		OnPressCallback onPressCallback;
		MenuItem* parent = nullptr;
	};

	class Menu
	{
	public:
		void Init(const MenuItem& item);

		void Update(float deltaTime);

		void Draw(sf::RenderWindow& window, sf::Vector2f position, sf::Vector2f origin);

		void PressOnSelectedItem();
		void GoBack();

		void SwitchToPreviousMenuItem();
		void SwitchToNextMenuItem();

		MenuItem& GetCurrentContext();
		const MenuItem& GetCurrentContext() const;

	private:
		void InitMenuItem(MenuItem& item);
		void SelectMenuItem(MenuItem& item);

	private:
		MenuItem rootItem;
		MenuItem* selectedItem = nullptr;
	};
}