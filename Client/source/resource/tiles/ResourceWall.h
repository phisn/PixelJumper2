#pragma once

#include <Client/source/resource/tiles/ResourceTileBase.h>

#pragma pack(push, 1)

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
				Game::TileId::Wall,
				NULL)
		{
		}

		bool make(ReadPipe*	 const pipe) override { return true; }
		bool save(WritePipe* const pipe) override { return true; }
	};
}

#pragma pack(pop)
