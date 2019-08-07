#pragma once

#include <Client/source/game/tiletrait/GameTileBase.h>
#include <Client/source/resource/TileResource.h>

#include <functional>

namespace Game
{
	namespace GameTileFactory
	{
		typedef std::function<GameTileBase* (Resource::Tile*)> TileCreator;
	
		bool Initialize();
		void Uninitialize();

		GameTileBase* create(Resource::Tile* const tileResource);
	}
}
