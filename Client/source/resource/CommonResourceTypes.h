#pragma once

#include <SFML/Main.hpp>
#include <SFML/System/Vector2.hpp>

namespace Resource
{
	typedef sf::Uint16 TilePosition;
	typedef sf::Uint8 TileSize;

	typedef sf::Vector2<TilePosition> VectorTilePosition;
	typedef sf::Vector2<TileSize> VectorTileSize;
}
