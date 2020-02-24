#pragma once

#include <SFML/System.hpp>

#include <Client/source/resource/CommonResourceTypes.h>

// TODO: rework all these definitions
namespace Editor
{
	typedef sf::Int64 TilePosition;
	typedef sf::Int32 TileSize;

	typedef sf::Vector2<TilePosition> VectorTilePosition;
	typedef sf::Vector2<TileSize> VectorTileSize;
}

namespace Resource
{
}

namespace Game
{
	typedef int TileIdentity;
	
	typedef Resource::TilePosition TilePosition;
	typedef Resource::TileSize TileSize;

	typedef sf::Vector2<TilePosition> VectorTilePosition;
	typedef sf::Vector2<TileSize> VectorTileSize;
}
