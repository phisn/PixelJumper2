#pragma once

#include <Client/source/editor/tile/TileBase.h>

#include <Client/source/game/tiles/Wall.h>

namespace Editor
{
	namespace Tile
	{
		class Wall
			:
			public TileBase
		{
		public:
			Wall(
				const sf::Vector2f position)
				:
				TileBase(
					TILE_WALL_COLOR,
					position)
			{
			}

			Resource::TileBase* create(
				const sf::Vector2u size,
				const sf::Vector2u position) const override 
			{
				return new Resource::WallTile();
			}
		};
	}
}