#pragma once

#include <SFML/System/Time.hpp>

namespace Game
{
	class TileContainer;
	class DynamicTile
	{
	public:
		void initialize(TileContainer* const);

		virtual void onLogic(const sf::Time time) = 0;
	};
}
