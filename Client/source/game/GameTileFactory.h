#pragma once

#include <Client/source/game/GameTileBase.h>
#include <Client/source/resource/TileResource.h>

namespace Game
{
	namespace GameTileFactory
	{
		GameTileBase* Create(Resource::Tile* const tileResource);
	}
}