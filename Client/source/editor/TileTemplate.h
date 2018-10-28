#pragma once

#include <SFML/Graphics.hpp>

namespace Editor
{
	class TileTemplate
	{
	public:
		TileTemplate(
			const sf::Color color)
			:
			color(color)
		{
		}

		// result == has context menu
		virtual bool onOpenContextMenu() = 0;

		sf::Color getColor() const
		{
			return color;
		}
	protected:
		sf::Color color;
	};
}