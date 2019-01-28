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
		Tile() = default;
		Tile(Tile&& tile) {
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
			AUTO Game::Tile::Id id;

			EDIT sf::Uint8 width;
			EDIT sf::Uint8 height;

			EDIT sf::Uint16 x;
			EDIT sf::Uint16 y;
			
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
		
		bool allocateContent()
		{
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

			return true;
		}

		bool save(
			WritePipe* const pipe) override
		{
			Header.id = Content->getTileId();
			Header.contentSize = (sf::Uint16) Content->getSize();

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
}