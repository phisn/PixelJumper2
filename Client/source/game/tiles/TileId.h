#pragma once

#include <SFML/Main.hpp>

namespace Game
{
	namespace Tile
	{
		enum class Id : sf::Uint16
		{
			Invalid = 0,

			Wall,
			
			_Length
		};
	}
}