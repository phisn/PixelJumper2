#pragma once

#include <Client/source/Common.h>
#include <Client/source/editor/tile/EditorTileBase.h>

#include <SFML/Graphics.hpp>

#include <variant>

namespace Editor
{
	namespace SelectionCache
	{
		struct Input
		{
			sf::Vector2i
				size,
				offset;
		};

		struct Area
		{
			// marking area (boxes)
			VectorTileSize size;
			VectorTilePosition offset;
		};

		struct Tile
		{
			std::vector<TileBase*> tiles;
		};

		struct Output
		{
			Output()
				:
				area(NULL)
			{
			}

			bool typeChanged;

			enum class Type
			{
				None,
				
				Area,
				Tile

			} type = Type::None;

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