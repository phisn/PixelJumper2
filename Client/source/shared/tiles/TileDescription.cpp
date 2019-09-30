#include "TileDescription.h"

#include <Client/source/shared/tiles/TileWall.h>
#include <Client/source/shared/tiles/TileExit.h>

#include <unordered_map>

namespace
{
	std::unordered_map<Shared::TileId, const Shared::TileDescription*> tileDescriptions;
}

namespace Shared
{
	void TileDescription::Initialize()
	{
		tileDescriptions[TileId::TileWall] = &TileWall;
		tileDescriptions[TileId::TileExit] = &TileExit;
	}

	const TileDescription* TileDescription::Find(TileId tile)
	{
		return tileDescriptions.at(tile);
	}
}
