#pragma once

#include <Client/source/editor/tile/EditorTileBase.h>

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
					position,
					Game::Tile::Id::Wall)
			{
			}

			Resource::TileBase* create(
				const Resource::VectorTileSize size,
				const Resource::VectorTilePosition position) const override 
			{
				return new Resource::WallTile();
			}

			bool adopt(const Resource::TileBase* const tile) override
			{
				return true; // ignore
			}
		};
	}
}