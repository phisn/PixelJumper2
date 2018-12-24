#pragma once

#include <map>

#include <Client/source/game/tiles/TileBase.h>
#include <Client/source/game/tiles/TileId.h>

/*

	Tile -> Handles functioning
	Tile_Settings -> Contains Tile Data
	Tile_Resource -> Handles load, saving of
		Data (settings) and creation of Tile

*/

namespace Game
{
	namespace Tile
	{
		namespace Manager
		{
			_Ret_maybenull_
			BaseResource* getTileResource(const Id tileId);
			void registerTileResource(
				const Id tileId, 
				BaseResource* resource);
			void clearTileResources();

			// Private ex. Easter Eggs or hidden tiles
			void registerAllPrivate();
			void registerAllPublic();
		}
	}
}