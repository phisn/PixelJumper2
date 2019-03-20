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
				sizeof(Content))
		{
		}

		bool make(ReadPipe*	const pipe) override 
		{ 
			return pipe->readValue(&Content);
		}

		bool save(WritePipe* const pipe) override 
		{ 
			return pipe->writeValue(&Content);
		}

		bool setup() override
		{
			return true;
		}

		struct
		{
			float density;
			float drag;

		} Content;
	};
}

#pragma pack(pop)
