#pragma once

#include "Common/Common.h"

#include "ComponentResource.h"
#include "EntityResource.h"
#include "PlayerResource.h"

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
			sf::Uint16 entityCount;
			sf::Uint16 componentCount;

			sf::Uint8 targetCount;
		};

	public:
		// does automatically obtain the responsibility for
		// pushed components
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

		std::vector<EntityResource> entities;
		std::vector<ComponentResource> components;

		std::vector<Resource::WorldId> targets;

		bool make(ReadPipe* const pipe) override
		{
			entities.clear();
			components.clear();
			targets.clear();

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
				components.emplace_back();

				if (!components.back().make(pipe))
					return false;
			}

			entities.resize(privateContent.entityCount);

			for (EntityResource& entity : entities)
				if (!entity.make(pipe))
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

			privateContent.entityCount = entities.size();
			privateContent.componentCount = components.size();
			privateContent.targetCount = targets.size();

			if (!pipe->writeValue(&privateContent))
			{
				return false;
			}

			for (ComponentResource& component : components)
				if (!component.save(pipe))
				{
					return false;
				}

			for (EntityResource& entity : entities)
				if (!entity.save(pipe))
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
