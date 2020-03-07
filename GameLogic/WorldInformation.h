#pragma once

#include "Common.h"
#include "Resource/WorldResource.h"

#include <SFML/Graphics/Rect.hpp>

#include <string>

namespace Game
{
	struct WorldInformation
	{
		static WorldInformation Create(const Resource::World* const resource)
		{
			WorldInformation info;

			info.worldId = resource->content.id;

			info.size = sf::Vector2i(
				resource->content.width,
				resource->content.height
			);

			info.defaultPlayerProperties = resource->content.defaultPlayerProperties;
			info.defaultProperties = resource->content.defaultProperties;

			info.defaultPlayerProperties.position = { 2, 2 };
			info.defaultPlayerProperties.viewWindow = { 0, 0, 50, 50 };

			return info;
		}

		std::wstring authorName;
		std::wstring mapName;

		Resource::WorldId worldId;
		sf::Vector2i size;

		Shared::WorldDefaultProperties defaultProperties;
		Shared::PlayerDefaultProperties defaultPlayerProperties;
	};
}
