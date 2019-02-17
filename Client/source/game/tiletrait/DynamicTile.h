#pragma once

#include <Client/source/game/GameTileBase.h>
#include <Client/source/game/GameTileContainer.h>

#include <SFML/System/Time.hpp>

namespace Game
{
	class DynamicTile
		:
		public GameTileBase
	{
	public:
		void initialize(TileContainer* const)
		{

		}

		virtual void onLogic(const sf::Time time) = 0;
		virtual void onDraw() = 0;
	};
}
