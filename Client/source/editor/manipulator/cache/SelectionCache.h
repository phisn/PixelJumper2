#pragma once

#include <SFML/Graphics.hpp>

#include <variant>

namespace Editor
{
	namespace SelectionCache
	{
		struct Input
		{
			sf::Vector2f
				size,
				offset;
		};

		struct Area
		{
			// marking area (boxes)
			sf::Vector2f
				size,
				offset;
		};

		struct Tile
		{
			std::vector<Tile*> tiles;
		};

		struct Output
		{
			Output()
				:
				area(NULL)
			{
			}

			enum class Type
			{
				Area,
				Tile

			} type;

			// independent from view
			// always shown as marker
			sf::Vector2f
				size,
				offset;

			union
			{
				Area* area;
				Tile* tile;
			};
		};
	}
}