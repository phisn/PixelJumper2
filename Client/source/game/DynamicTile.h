#pragma once

#include <Client/source/game/GameTileContainer.h>

#include <SFML/System/Time.hpp>

namespace Game
{
	class DynamicTile
	{
	public:
		void initialize(TileContainer* const)
		{

		}

		virtual void onLogic(const sf::Time time) = 0;
	};
}
