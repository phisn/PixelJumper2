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

		bool make(ReadPipe*	 const pipe) override { return true; }
		bool save(WritePipe* const pipe) override { return true; }
	};
}
