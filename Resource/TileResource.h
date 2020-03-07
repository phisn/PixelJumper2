#pragma once

#include "Common.h"

#include "ResourceBase.h"
#include "TrivialResource.h"

#pragma pack(push, 1)

namespace Resource
{
	struct TileContent
	{
		TileSize width, height;
		TilePosition x, y;

		sf::Uint16 componentIndex;

	};

	typedef TrivialResource<TileContent> Tile;
}

#pragma pack(pop)
