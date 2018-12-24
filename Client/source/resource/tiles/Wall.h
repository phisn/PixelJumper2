#pragma once

#include <Client/source/resource/tiles/TileBase.h>

namespace Resource
{
	class WallTile
		:
		public TileBase
	{
	public:
		WallTile()
			:
			TileBase(
				Game::Tile::Id::Wall,
				NULL)
		{
		}
	};
}
