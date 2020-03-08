#include "GameWorld.h"

#include "tiletrait/InitializableTile.h"

namespace Game
{
	bool World::initializeTiles()
	{
		for (InitializableTile* const tile : environment.getTileType<InitializableTile>())
			if (!tile->initialize(this))
			{
				return false;
			}

		return true;
	}
}
