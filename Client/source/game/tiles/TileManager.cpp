#include "TileManager.h"

#include <Client/source/game/tiles/Wall.h>

namespace
{
	std::map<
		Game::Tile::Id, 
		Game::Tile::BaseResource*> creators;
}

namespace Game
{
	namespace Tile
	{
		BaseResource* Manager::getTileResource(
			const Id tileId)
		{
			decltype(creators)::iterator iterator
				= creators.find(tileId);
			if (iterator == creators.end())
			{
				return NULL;
			}
			else
			{
				return iterator->second;
			}
		}

		void Manager::registerTileResource(
			const Id tileId, 
			BaseResource* resource)
		{
			creators[tileId] = resource;
		}

		void Manager::clearTileResources()
		{
			creators.clear();
		}

		void Manager::registerAllPrivate()
		{
			registerAllPublic();
		}

		void Manager::registerAllPublic()
		{
			WallResource::registerResource();
		}
	}
}
