#include "TileResource.h"

#include <Client/source/shared/tiles/TileDescription.h>

namespace Resource
{
	TileInstanceWrapper::TileInstanceWrapper(const Shared::TileID id)
		:
		id(id)
	{
		instance = Shared::TileDescription::Find(id)->creation.createResourceTile();
	}
	
	bool TileInstanceWrapper::make(ReadPipe* const pipe)
	{
		if (!pipe->readValue(&id))
		{
			return false;
		}
		
		// normally not possible
		if (instance)
		{
			Log::Warning(L"Tile resource wrapper instance was not null on make");

			delete instance;
		}

		instance = Shared::TileDescription::Find(id)->creation.createResourceTile();

		return instance->make(pipe);
	}
}
