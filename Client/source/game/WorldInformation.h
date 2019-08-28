#pragma once

#include <Client/source/resource/WorldResource.h>

#include <string>

namespace Game
{
	struct WorldInformation
	{
		static WorldInformation Create(const Resource::World* const resource)
		{
			WorldInformation info;

			info.worldId = resource->HeaderIntro.worldID;

			info.authorName = resource->HeaderAuth.authorName;
			info.mapName = resource->HeaderAuth.mapName;

			info.size = sf::Vector2i(
				resource->HeaderProperties.width,
				resource->HeaderProperties.height
			);

			info.defaultPlayerProperties.beginMovement = { 0, 0 };
			info.defaultPlayerProperties.beginPosition = { 0, -1 };

			info.defaultPlayerProperties.friction = 0;
			info.defaultPlayerProperties.speed = 0;
			info.defaultPlayerProperties.weight = 0;

			info.defaultWorldProperties.airResistance = 0;
			info.defaultWorldProperties.gravity = { 0, 0 };

			return info;
		}

		std::wstring authorName;
		std::wstring mapName;

		Resource::WorldId worldId;
		sf::Vector2i size;

		struct
		{
			sf::Vector2f gravity;
			float airResistance;

		} defaultWorldProperties;

		struct
		{
			sf::Vector2i beginPosition;
			sf::Vector2f beginMovement;

			float speed, weight, friction;

		} defaultPlayerProperties;
	};
}
