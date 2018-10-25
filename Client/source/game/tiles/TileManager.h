#pragma once

#include <map>

#include <Client/source/game/tiles/TileBase.h>


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
		typedef BaseResource* (*TileCreator)();

		enum class Id
		{
			InvalidTile = 0,

			Wall
		};

		namespace Manager
		{
			std::map<Id, TileCreator> creators;

			void registerTile(Id, TileCreator);
		}
	}
}