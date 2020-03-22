#include "WorldResource.h"

namespace Resource
{
	bool World::validateAuth()
	{
		return true;
	}

	bool World::validateInnerContent()
	{
		return true;
	}

	bool World::validateTiles()
	{
		return true;
	}

	sf::Uint32 World::generateCheckSum()
	{
		return 0;
	}
}