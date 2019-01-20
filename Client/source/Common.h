#pragma once

#include <SFML/System.hpp>

namespace Editor
{
	typedef sf::Int64 TilePosition;
	typedef sf::Int32 TileSize;

	typedef sf::Vector2<TilePosition> VectorTilePosition;
	typedef sf::Vector2<TileSize> VectorTileSize;
}

namespace Resource
{
	typedef sf::Int16 TilePosition;
	typedef sf::Int16 TileSize;

	typedef sf::Vector2<TilePosition> VectorTilePosition;
	typedef sf::Vector2<TileSize> VectorTileSize;
}

namespace Game
{
	typedef Resource::TilePosition TilePosition;
	typedef Resource::TileSize TileSize;

	typedef sf::Vector2<TilePosition> VectorTilePosition;
	typedef sf::Vector2<TileSize> VectorTileSize;
}
