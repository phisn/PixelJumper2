#pragma once

#include <Client/source/game/GameState.h>

namespace Game
{
	class TileContainer;
	class SynchronizableTile
	{
	public:
		void initialize(TileContainer* const container);

		virtual GameState* getState() = 0;
	};
}
