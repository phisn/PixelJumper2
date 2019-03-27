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
				Resource::WallTile* const tile = new Resource::WallTile();
				
				tile->Content.density = 1.f; // TODO: Make Variable
				tile->Content.friction = 1.f;

				return tile;
			}

			bool adopt(const Resource::TileBase* const tile) override
			{
				return true; // ignore
			}

			bool equals(
				TileBase* const tile) const override
			{
				return id == tile->id; // && ((Wall*)tile)->;
			}
		};
	}
}