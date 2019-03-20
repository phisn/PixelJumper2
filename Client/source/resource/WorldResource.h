#pragma once

#include <Client/source/resource/TileResource.h>
#include <Client/source/resource/ResourceBase.h>

#include <SFML/Main.hpp>

#include <string>
#include <vector>

#define WORLD_MAGIC 0xbb'22'cc'33

#pragma pack(push, 1)

namespace Resource
{
	typedef sf::Uint32 WorldId;

	class World
		:
		public Resource::ResourceBase
	{
		typedef unsigned char AuthorNameSize;	// 0 - 255
		typedef unsigned char MapNameSize;		// 0 - 255
	public:
		struct
		{
			AUTO sf::Uint32 magic = WORLD_MAGIC;
			EDIT WorldId worldID = NULL;
		} HeaderIntro;

		struct
		{
			EDIT std::wstring authorName;
			EDIT std::wstring mapName;
		} HeaderAuth = { };

		struct
		{
			EDIT sf::Uint16 width, height;

			AUTO sf::Uint16 tileCount;
			AUTO sf::Uint32 tileCheckSum;
		} HeaderProperties = { };

		bool make(ReadPipe* const pipe) override;
		bool save(WritePipe* const pipe) override;

		bool setup()
		{
			HeaderIntro.magic = WORLD_MAGIC;

			if (!validateHeaderIntro() ||
				!validateHeaderAuth())
			{
				return false;
			}

			HeaderProperties.tileCount = (sf::Int16) TileContainer.size();
			HeaderProperties.tileCheckSum = generateCheckSum();

			if (!validateHeaderProperties())
			{
				return false;
			}

			for (Tile& tile : TileContainer)
				if (!tile.setup())
				{
					return false;
				}

			return true;
		}

		EDIT std::vector<Resource::Tile> TileContainer;
	private:
		const sf::Uint64 HEADER_SIZE
			= sizeof(HeaderIntro)
			+ 4 // headerAuth
			+ sizeof(HeaderProperties);

		sf::Uint32 generateCheckSum();

		bool readHeaderIntro(
			ReadPipe* const pipe);
		bool readHeaderAuth(
			ReadPipe* const pipe);
		bool readHeaderProperties(
			ReadPipe* const pipe);

		bool validateHeaderIntro() const;
		bool validateHeaderAuth() const;
		bool validateHeaderProperties() const;
	};
}

#pragma pack(pop)
