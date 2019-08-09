#include "TileDescription.h"

#include <unordered_map>

namespace
{
	std::unordered_map<Shared::TileId, Shared::TileDescription*> tileDescriptions;
}

namespace Shared
{
	template <>
	const TileDescription* TileDescription::Find<TileDescription>(TileId tile)
	{
		return tileDescriptions.at(tile);
	}
}
