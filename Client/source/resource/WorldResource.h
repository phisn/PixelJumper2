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
	// moved to playerresource
	// typedef sf::Uint32 WorldId;

	class World
		:
		public Resource::ResourceBase
	{
		struct PrivateContent
		{
			sf::Uint16 tileInstanceCount;
			sf::Uint16 tileCount;

			sf::Uint8 targetCount;
		};

	public:
		~World()
		{
		}

		struct Content
		{
			sf::Uint32 checksum;

			WorldId id;
			PlayerID author;

			sf::Uint16 width, height;

			Shared::WorldDefaultProperties defaultProperties;
			Shared::PlayerDefaultProperties defaultPlayerProperties;

		} content;

		std::vector<TileInstanceWrapper> tileInstances;
		std::vector<Tile> tiles;

		std::vector<Resource::WorldId> targets;
		
		bool make(ReadPipe* const pipe) override
		{
			if (!pipe->readValue(&content))
			{
				return false;
			}

			PrivateContent privateContent;
			if (!pipe->readValue(&privateContent))
			{
				return false;
			}

			tileInstances.resize(privateContent.tileInstanceCount);
			for (TileInstanceWrapper& instance : tileInstances)
				if (!instance.make(pipe))
				{
					return false;
				}

			tiles.resize(privateContent.tileCount);

			for (Tile& tile : tiles)
				if (!tile.make(pipe))
				{
					return false;
				}

			targets.resize(privateContent.targetCount);

			for (Resource::WorldId& worldID : targets)
				if (!pipe->readValue(&worldID))
				{
					return false;
				}

			return true;
		}

		bool save(WritePipe* const pipe) override
		{
			if (!pipe->writeValue(&content))
			{
				return false;
			}

			PrivateContent privateContent;

			privateContent.tileInstanceCount = tileInstances.size();
			privateContent.tileCount = tiles.size();
			privateContent.targetCount = targets.size();

			if (!pipe->writeValue(&privateContent))
			{
				return false;
			}

			for (TileInstanceWrapper& instance : tileInstances)
				if (!instance.save(pipe))
				{
					return false;
				}

			for (Tile& tile : tiles)
				if (!tile.save(pipe))
				{
					return false;
				}

			for (Resource::WorldId& worldID : targets)
				if (!pipe->writeValue(&worldID))
				{
					return false;
				}
			
			return true;
		}

		bool setup() override
		{
			return validate();
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
}

#pragma pack(pop)
