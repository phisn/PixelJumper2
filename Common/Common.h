#pragma once

#include "OperatorDefinitions.h"
#include "SharedDefinitions.h"
#include "Tiles.h"

#include "Resource/CommonResourceTypes.h"

namespace Editor
{
	typedef sf::Int64 TilePosition;
	typedef sf::Int32 TileSize;

	typedef sf::Vector2<TilePosition> VectorTilePosition;
	typedef sf::Vector2<TileSize> VectorTileSize;
}

namespace Game
{
	typedef int TileIdentity;

	typedef Resource::TilePosition TilePosition;
	typedef Resource::TileSize TileSize;

	typedef sf::Vector2<TilePosition> VectorTilePosition;
	typedef sf::Vector2<TileSize> VectorTileSize;
}

