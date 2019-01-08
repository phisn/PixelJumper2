#pragma once

#include <Client/source/game/tiles/TileManager.h>

#include <Client/source/resource/TileFactory.h>
#include <Client/source/resource/ResourceBase.h>

#include <Client/source/resource/tiles/TileBase.h>

namespace Resource
{
	class Tile
		:
		public Resource::ResourceBase
	{
	public:
		~Tile()
		{
			if (Content) delete Content;
		}

		struct
		{
			AUTO Game::Tile::Id id;

			EDIT sf::Uint8 width;
			EDIT sf::Uint8 height;

			AUTO sf::Uint16 contentSize;
		} Header = { };

		_Maybenull_
		EDIT TileBase* Content = NULL;

		bool make(
			ReadPipe* const pipe) override
		{
			if (pipe->getSize() < sizeof(Header))
			{
				return false;
			}

			if (!readHeader(pipe))
			{
				return false;
			}

			if (Content != NULL &&
				Content->getTileId() != Header.id)
			{
				delete Content;
				Content = NULL;
			}

			if (Content == NULL)
			{
				Content = TileFactory::Create(Header.id);

				if (Content == NULL)
				{
					return false;
				}
			}

			Content->make(pipe);
		}

		bool save(
			WritePipe* const pipe) override
		{
			Header.id = Content->getTileId();

			if ( !writeHeader(pipe) )
			{
				return false;
			}

			if (Content == NULL)
			{
				return false;
			}

			if (!Content->save(pipe))
			{
				return false;
			}

			return true;
		}

	private:
		bool readHeader(
			ReadPipe* const pipe) const
		{
			return pipe->readValue(&Header)
				&& validateHeader();
		}

		bool writeHeader(
			WritePipe* const pipe)
		{
			return validateHeader() 
				&& pipe->writeValue(&Header);
		}

		bool validateHeader() const
		{
			return Header.id > Game::Tile::Id::Invalid
				&& Header.width == 0
				&& Header.height == 0
				&& Header.contentSize == 0;
		}
	};

	struct _N_Tile
		:
		public Resource::ResourceBase
	{
		// NOT RESPONSIBLE FOR DELETING TILE
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