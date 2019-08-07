#include "TileDescription.h"

#include <unordered_map>

namespace
{
	std::unordered_map<Shared::TileId, Shared::TileDescription*> tileDescriptions;
}

namespace Shared
{
	const TileDescription* TileDescription::find(TileId tile)
	{
		return tileDescriptions.at(tile);
	}
}
