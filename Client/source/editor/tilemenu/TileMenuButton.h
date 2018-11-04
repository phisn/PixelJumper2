#pragma once

#include <Client/source/menu/element/SimpleButton.h>

#include <functional>

namespace Editor
{
	class TileMenuButton
		:
		public MENU::SimpleButton
	{
	public:
		struct Style
		{
			sf::Vector2f
				size,
				position;

			std::wstring label;
			sf::Color 
				fillColor, 
				clickColor;

			MENU::SimpleButton::Style makeStyle() const
			{
				MENU::SimpleButton::Style style = { };

				style.size = size;
				style.position = position;
				
				style.enter_fillColor = fillColor;
				style.enter_outlineColor = sf::Color::Color(150, 150, 150);
				style.enter_outlineThickness = 0.2f;


				style.click_fillColor = clickColor;
				style.click_outlineColor = { };
				style.click_outlineThickness = 0.0f;


				style.default_fillColor = fillColor;
				style.click_outlineColor = { };
				style.click_outlineThickness = 0.0f;

				return style;
			}
		};

		TileMenuButton(
			const Style style)
			:
			MENU::SimpleButton( style.makeStyle() ),
			label(style.label)
		{
		}

		void setOnClick(
			const std::function<void()> callback)
		{
			this->callback = callback;
		}

		void onLogic(
			const sf::Time time) override { }
	private:
		std::function<void()> callback = callback;

		void onMouseClick() override
		{
			if (callback) callback();
		}

		std::wstring label;
	};
}
