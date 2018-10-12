#pragma once

#include <SFML/System/Time.hpp>

namespace Game
{
	namespace Tile
	{
		class Timed
		{
		public:
			virtual void onTime(sf::Time time) = 0;
		};
	}
}
