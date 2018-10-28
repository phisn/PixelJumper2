#pragma once

#include <Client/source/editor/tile/TileBase.h>

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

		sf::Color getColor() const
		{
			return color;
		}

		virtual TileBase* create() = 0;
	protected:
		sf::Color color;
	};
}