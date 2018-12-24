#pragma once

#include <Client/source/resource/TileResource.h>
#include <Client/source/resource/ResourceBase.h>

#include <SFML/Main.hpp>

#include <string>
#include <vector>

#define WORLD_MAGIC 0xbb'22'cc'33

namespace Resource
{
	class World
		:
		public Resource::ResourceBase
	{
		typedef unsigned char AuthorNameSize;	// 0 - 255
		typedef unsigned char MapNameSize;		// 0 - 255
	public:
		struct
		{
			sf::Uint32 magic = WORLD_MAGIC;
			sf::Uint32 worldID = NULL;
		} HeaderIntro;

		struct
		{
			std::wstring authorName;
			std::wstring mapName;
		} HeaderAuth = { };

		struct
		{
			sf::Uint16 width, height;

			sf::Uint16 tileCount;
			sf::Uint32 tileCheckSum;
		} HeaderProperties = { };

		std::vector<Resource::Tile> tiles;
	private:
		const sf::Uint64 HEADER_SIZE
			= sizeof(HeaderIntro)
			+ 4 // headerAuth
			+ sizeof(HeaderProperties);

		sf::Uint32 generateCheckSum()
		{
			sf::Uint32 result = (1 << 16) + (1 << 8) + 3;

			result += HeaderProperties.width;
			result -= HeaderProperties.height;
			result += HeaderProperties.tileCount;

			result *= HeaderIntro.worldID;
			result /= HeaderAuth.authorName.size();
			result -= HeaderAuth.mapName.size();

			for (Resource::Tile& tile : tiles)
			{ // TODO: check if not bad
				result *= (sf::Uint16) tile.Header.id;
				result += tile.Header.height;
				result *= tile.Header.width;
				result += tile.Header.contentSize;
			}

			return result;
		}

		bool make(
			ReadPipe* const pipe) override
		{
			if (pipe->getSize() < HEADER_SIZE)
			{
				return false;
			}

			////////// Header
			if (!readHeaderIntro(pipe))
			{
				return false;
			}

			if (!readHeaderAuth(pipe))
			{
				return false;
			}

			if (!readHeaderProperties(pipe))
			{
				return false;
			}
			////////// Conent
			tiles.clear();
			tiles.resize(HeaderProperties.tileCount);

			for (Tile& tile : tiles)
				if (!tile.make(pipe))
				{
					return false;
				}

			//////////
			return HeaderProperties.tileCheckSum == generateCheckSum();
		}

		bool save(
			WritePipe* const pipe) override
		{

		}

		bool readHeaderIntro(
			ReadPipe* const pipe)
		{
			if (!pipe->readValue( &HeaderIntro ))
			{
				return false;
			}

			return validateHeaderIntro();
		}

		bool readHeaderAuth(
			ReadPipe* const pipe)
		{
			if (!pipe->readString<AuthorNameSize>( &HeaderAuth.authorName ))
			{
				return false;
			}

			if (!pipe->readString<MapNameSize>(&HeaderAuth.authorName))
			{
				return false;
			}

			return validateHeaderAuth();
		}

		bool readHeaderProperties(
			ReadPipe* const pipe)
		{
			if (pipe->readValue( &HeaderProperties ))
			{
				return false;
			}

			return validateHeaderProperties();
		}

		bool validateHeaderIntro() const
		{
			return HeaderIntro.magic == WORLD_MAGIC
				&& HeaderIntro.worldID != 0;
		}

		bool validateHeaderAuth() const
		{
			return HeaderAuth.authorName.size() > 0
				&& HeaderAuth.mapName.size() > 0;
		}

		// checksum not tested
		bool validateHeaderProperties() const
		{
			return HeaderProperties.width != 0
				&& HeaderProperties.height != 0
				&& HeaderProperties.tileCount != 0;
		}
	};

	struct _N_World
		:
		public ResourceBase
	{
		struct
		{
			struct
			{
				sf::Uint32 magic = WORLD_MAGIC;
				sf::Uint32 worldId;
			} begin;

			struct
			{
				std::wstring author;
				std::wstring name;
			} auth;

			struct
			{
				sf::Uint32 width;
				sf::Uint32 height;

				sf::Int32 tileCount;
				sf::Int32 contentSize;
			} prop;
		} header;

		struct
		{
			std::vector<_N_Tile> tileResources;

		} content;

		bool writeToBuffer(
			RESOURCE::ByteWriter* buffer) override
		{
			if (!setupWriteData())
			{
				return false;
			}

			if (!validateHeader() ||
				!validateContent())
			{
				return false;
			}

			writeHeader(buffer);

			if (!writeContent(buffer))
			{
				return false;
			}

			return true;
		}

		bool readFromBuffer(
			ByteReader* buffer) override
		{
			if (!readHeader(buffer))
			{
				return false;
			}

			if (!validateHeader())
			{
				return false;
			}

			if (!readContent(buffer))
			{
				return false;
			}

			return validateContent();
		}

		bool validateHeader() const
		{
			if (header.auth.author.length() > 0xff ||
				header.auth.name.length() > 0xff)
			{
				return false;
			}

			return header.begin.magic == WORLD_MAGIC;
		}

		bool validateContent() const
		{
			return true;
		}
	private:
		bool readHeader(
			ByteReader* buffer)
		{
			if (!buffer->readValue( &header.begin ))
			{
				return false;
			}

			if (!buffer->readString(&header.auth.author) ||
				!buffer->readString(&header.auth.name))
			{
				return false;
			}

			return buffer->readValue(&header.prop);
		}

		bool readContent(
			ByteReader* buffer)
		{
			for (int i = 0; i < header.prop.tileCount; ++i)
			{
				content.tileResources.emplace_back();

				if (!content.tileResources.back().readFromBuffer(
						buffer))
				{
					return false;
				}
			}

			return true;
		}

		bool setupWriteData()
		{
			header.begin.magic = WORLD_MAGIC;

			header.prop.contentSize;
			header.prop.tileCount = (const sf::Uint32) content.tileResources.size();

			return true;
		}

		void writeHeader(
			ByteWriter* buffer)
		{
			buffer->writeValue( &header.begin );

			buffer->write(header.auth.author);
			buffer->write(header.auth.name);

			buffer->writeValue( &header.prop );
		}

		bool writeContent(
			RESOURCE::ByteWriter* buffer)
		{
			for (_N_Tile& resource : content.tileResources)
				if (!resource.writeToBuffer(
						buffer))
				{
					return false;
				}

			return true;
		}
	};
}
