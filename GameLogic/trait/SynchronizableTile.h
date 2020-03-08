#pragma once

#include "GameState.h"

namespace Game
{
	class SynchronizableTile
	{
	public:
		// currently not implemented
		virtual GameState* getState() = 0;
	};
}
