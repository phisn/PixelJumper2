#include "GameWorld.h"

#include "trait/InitializableTrait.h"

namespace Game
{
	bool World::initializeTiles()
	{
		for (InitializableTraitHandler* const tile : environment.getInitializableTrait())
			if (!tile->initialize(this))
			{
				return false;
			}

		return true;
	}
}
