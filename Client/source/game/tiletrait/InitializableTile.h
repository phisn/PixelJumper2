#pragma once

#include <Client/source/game/GameWorld.h>

namespace Game
{
	class InitializableTile
	{
	public:
		// can fail
		virtual bool initialize(World* const world) = 0;
	};
}
