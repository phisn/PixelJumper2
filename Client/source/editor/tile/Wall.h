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
				const VectorTilePosition position)
				:
				TileBase(
					TILE_WALL_COLOR,
					position)
			{
			}

			Resource::TileBase* create(
				const Resource::VectorTileSize size,
				const Resource::VectorTilePosition position) const override 
			{
				return new Resource::WallTile();
			}
		};
	}
}