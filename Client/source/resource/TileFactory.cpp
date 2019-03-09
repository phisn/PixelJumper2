#include "TileFactory.h"

namespace Resource
{
	std::map<Game::TileId, TileFactory::Creator> creators;

	TileBase* Resource::TileFactory::Create(
		const Game::TileId ID)
	{
		return creators[ID]();
	}

	void Resource::TileFactory::Register(
		const Game::TileId ID, 
		const Creator creator)
	{
		creators[ID] = creator;
	}
}
