#pragma once

#include <Client/source/game/tiles/TileBase.h>
#include <Client/source/game/tiles/TileManager.h>

#include <Client/source/resource/ResourceBase.h>

namespace Resource
{
	struct TileResource
		:
		public Resource::Base
	{
		GAME::Tile::Base* tile;

		struct
		{
			GAME::Tile::Id id;
			sf::Uint32 size;

		} data;

		bool writeToBuffer(
			ByteWriter* buffer) override
		{
			GAME::Tile::BaseResource* resource = GAME::Tile::Manager::getTileResource(
				tile->getId());
			if (resource == NULL)
			{
				return false;
			}

			data.id = tile->getId();
			data.size = resource->getSize();

			buffer->writeValue(&data);

			resource->loadFromTile(tile);
			return resource->writeToBuffer(buffer);
		}

		bool readFromBuffer(
			ByteReader* buffer) override
		{
			if (!buffer->readValue(&data))
			{
				return false;
			}

			GAME::Tile::BaseResource* resource = GAME::Tile::Manager::getTileResource(
				data.id);
			if (resource == NULL)
			{
				return false;
			}

			if (!buffer->readValue(&data))
			{
				return false;
			}

			if (!resource->readFromBuffer(buffer))
			{
				return false;
			}

			return NULL !=
				(
					tile = resource->create()
				);
		}
	};

}