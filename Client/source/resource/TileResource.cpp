#include "TileResource.h"

#include <Client/source/shared/tiles/TileDescription.h>

namespace Resource
{
	bool Tile::allocateContent()
	{
		if (Content != NULL &&
			Content->getTileId() != Header.id)
		{
			delete Content;
			Content = NULL;
		}

		if (Content == NULL)
		{
			Content = Shared::TileDescription::Find(Header.id)->creation.createResourceTile();

			if (Content == NULL)
			{
				return false;
			}
		}

		return true;
	}
}
