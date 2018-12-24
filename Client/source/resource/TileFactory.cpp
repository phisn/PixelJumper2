#include "TileFactory.h"

namespace Resource
{
	std::map<Game::Tile::Id, TileFactory::Creator> creators;

	TileBase* Resource::TileFactory::Create(
		const Game::Tile::Id ID)
	{
		return creators[ID]();
	}

	void Resource::TileFactory::Register(
		const Game::Tile::Id ID, 
		const Creator creator)
	{
		creators[ID] = creator;
	}
}
