#include "GameTileFactory.h"

// #include <Client/source/game/tile/GameWallTile.h>

#include <unordered_map>

namespace
{
	std::unordered_map<Game::TileId, Game::GameTileFactory::TileCreator> tileCreators;
}

#define INSERT_TILE_CREATOR_DIFFER(gname, ename) \
	tileCreators[Game::TileId::##gname] = Game::ename##Tile::Create
#define INSERT_TILE_CREATOR(name) \
	INSERT_TILE_CREATOR_DIFFER(name, name)

namespace Game
{
	bool GameTileFactory::Initialize()
	{
		// INSERT_TILE_CREATOR(TileWall);

		return true;
	}

	void GameTileFactory::Uninitialize()
	{
		tileCreators.clear();
	}

	GameTileBase* GameTileFactory::create(
		Resource::Tile* const tileResource)
	{
		return tileCreators[tileResource->Header.id](tileResource);
	}
}
