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
			AUTO sf::Uint32 magic = WORLD_MAGIC;
			EDIT sf::Uint32 worldID = NULL;
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

		EDIT std::vector<Resource::Tile> TileContainer;
	private:
		const sf::Uint64 HEADER_SIZE
			= sizeof(HeaderIntro)
			+ 4 // headerAuth
			+ sizeof(HeaderProperties);

		sf::Uint32 generateCheckSum();

		bool make(ReadPipe* const pipe) override;
		bool save(WritePipe* const pipe) override;

		bool readHeaderIntro(
			ReadPipe* const pipe);
		bool readHeaderAuth(
			ReadPipe* const pipe);
		bool readHeaderProperties(
			ReadPipe* const pipe);

		bool writeHeaderIntro(
			WritePipe* const pipe);
		bool writeHeaderAuth(
			WritePipe* const pipe);
		bool writeHeaderProperties(
			WritePipe* const pipe);

		bool validateHeaderIntro() const;
		bool validateHeaderAuth() const;
		bool validateHeaderProperties() const;
	};
}
