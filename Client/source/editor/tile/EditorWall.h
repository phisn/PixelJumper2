#pragma once

#include <Client/source/editor/tile/EditorTileBase.h>

#include <Client/source/game/tile/GameWallTile.h>

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
					Game::WallTile::COLOR,
					position,
					Game::TileId::Wall)
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