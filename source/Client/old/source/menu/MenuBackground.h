#pragma once

#include <Client/source/menu/MenuElementBase.h>

namespace Menu
{
	class Background
		:
		public ElementBase
	{
	public:
		Background()
		{
			color.addListener(
				[this](const sf::Color oldColor,
					   const sf::Color newColor)
				{
					rect.setFillColor(newColor);
				});

			color = sf::Color::Transparent;
		}

		Property<sf::Color> color;

		void onDraw(sf::RenderTarget* const target) const override
		{
			target->draw(rect);
		}

	private:
		void updateOwnGraphics() override
		{
			rect.setPosition(convertPositionVTR({ 0, 0 }));
			rect.setSize(size);
		}

		void moveOwnGraphics(const sf::Vector2f offset) override
		{
			rect.move(offset);
		}

		sf::RectangleShape rect;
	};
}