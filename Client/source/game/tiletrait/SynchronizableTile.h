#pragma once

#include <Client/source/game/GameState.h>

namespace Game
{
	class SynchronizableTile
	{
	public:
		// currently not implemented
		virtual GameState* getState() = 0;
	};
}
