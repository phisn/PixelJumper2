#include "TileDescription.h"

#include <Client/source/shared/tiles/TileWall.h>
#include <Client/source/shared/tiles/TileExit.h>

#include <unordered_map>

namespace
{
	std::unordered_map<Shared::TileID, const Shared::TileDescription*> tileDescriptions;
}

namespace Shared
{
	void TileDescription::Initialize()
	{
		tileDescriptions[TileID::TileWall] = &TileWall;
		tileDescriptions[TileID::TileExit] = &TileExit;
	}

	const TileDescription* TileDescription::Find(TileID tile)
	{
		return tileDescriptions.at(tile);
	}
}
