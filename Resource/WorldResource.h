#pragma once

#include "Common.h"

#include "ComponentResource.h"
#include "PlayerResource.h"
#include "TileResource.h"

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
			sf::Uint16 componentCount;
			sf::Uint16 tileCount;

			sf::Uint8 targetCount;
		};

	public:
		// does automatically obtain the responsibility for
		// pushed components
		~World()
		{
			for (ComponentResource* const component : components)
				delete component;
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

		std::vector<ComponentResource*> components;
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

			components.reserve(privateContent.componentCount);
			for (int i = 0; i < privateContent.componentCount; ++i)
			{
				ComponentID id;
				if (!pipe->readValue(&id))
					return false;

				components.push_back(ComponentResource::Create(id));

				if (!components.back()->make(pipe))
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

			privateContent.componentCount = components.size();
			privateContent.tileCount = tiles.size();
			privateContent.targetCount = targets.size();

			if (!pipe->writeValue(&privateContent))
			{
				return false;
			}

			for (ComponentResource* const component : components)
			{
				const ComponentID componentID = component->getID();
				if (!pipe->writeValue(&componentID))
					return false;

				if (!component->save(pipe))
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
