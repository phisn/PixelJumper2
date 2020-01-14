
#pragma once

#include <Client/source/resource/PlayerResource.h>
#include <Client/source/resource/ResourceBase.h>
#include <Client/source/resource/TileResource.h>

#include <Client/source/shared/WorldDefinitions.h>

#include <SFML/Main.hpp>

#include <string>
#include <vector>

#pragma pack(push, 1)

namespace Resource
{
	typedef sf::Uint32 WorldId;

	class World
		:
		public Resource::ResourceBase
	{
		struct PrivateContent
		{
			sf::Uint32 checksum;

			sf::Uint16 tileInstanceCount;
			sf::Uint16 tileCount;
		};

	public:
		~World()
		{
			for (TileInstanceWrapper* const instance : tileInstances)
				delete instance;
		}

		struct Content
		{
			WorldId id;
			PlayerID author;

			Shared::WorldDefaultProperties defaultProperties;
			Shared::PlayerDefaultProperties defaultPlayerProperties;

			sf::Uint16 width, height;

		} content;

		std::vector<TileInstanceWrapper*> tileInstances;
		std::vector<Tile> tiles;

		bool make(ReadPipe* const pipe) override
		{
			if (!pipe->readValue(&content))
			{
				return false;
			}

			for (TileInstanceWrapper* const instance : tileInstances)
				if (!instance->make(pipe))
				{
					return false;
				}

			for (Tile& tile : tiles)
				if (!tile.make(pipe))
				{
					return false;
				}

			return true;
		}

		bool save(WritePipe* const pipe) override
		{
		}

		bool setup() override
		{
		}

		bool validate() override
		{
			return validateAuth()
				&& validateInnerContent()
				&& validateTiles();
		}

	private:
		bool validateAuth();
		bool validateInnerContent();
		bool validateTiles();

		sf::Uint32 generateCheckSum();
	};

	/*
	class _N_World
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

			AUTO sf::Uint16 tileContentCount;
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
	*/
}

#pragma pack(pop)
