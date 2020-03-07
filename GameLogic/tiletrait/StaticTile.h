#pragma once

#include "TileContent.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

namespace Game
{
	class StaticTile
	{
	public:
		StaticTile(
			const sf::Color color, 
			const TileContent& content)
			:
			color(color),
			content(content)
		{
		}

		const sf::Color color;
		const TileContent& content;
	};
}