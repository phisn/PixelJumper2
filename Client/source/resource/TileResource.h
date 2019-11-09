#pragma once

#include <Client/source/resource/CommonResourceTypes.h>
#include <Client/source/resource/ResourceBase.h>
#include <Client/source/resource/ResourceTileContentBase.h>

#pragma pack(push, 1)

namespace Resource
{
	class Tile
		:
		public Resource::ResourceBase
	{
	public:
		Tile() = default;
		Tile(Tile&& tile) noexcept
		{
			memcpy(
				(char*) &Header, 
				(char*) &tile.Header, 
				sizeof(Header)
			);

			Content = tile.Content;
			tile.Content = NULL;
		}

		~Tile()
		{
			if (Content) delete Content;
		}

		struct
		{
			AUTO Shared::TileId id;

			EDIT TileSize width;
			EDIT TileSize height;

			EDIT TilePosition x;
			EDIT TilePosition y;
			
			AUTO sf::Uint16 contentSize; // probably does not exceed

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

			if (Header.contentSize == NULL)
			{ // skip content generation
				return true;
			}

			if (!allocateContent())
			{
				return false;
			}

			return Content->make(pipe);
		}
		
		bool allocateContent();

		bool save(
			WritePipe* const pipe) override
		{
			if (!setup())
			{
				return false;
			}

			if (!pipe->writeValue(&Header))
			{
				return false;
			}

			if (!Content->save(pipe))
			{
				return false;
			}

			return true;
		}

		bool setup() override
		{
			if (Content == NULL)
			{
				return false;
			}

			Header.id = Content->getTileId();
			Header.contentSize = (sf::Uint16) Content->getContentSize();

			return validateHeader();
		}

	private:
		bool readHeader(
			ReadPipe* const pipe) const
		{
			return pipe->readValue(&Header)
				&& validateHeader();
		}

		bool validateHeader() const
		{
			return Header.id > Shared::TileId::_Invalid
				&& Header.width != 0
				&& Header.height != 0;
		}
	};
}

#pragma pack(pop)