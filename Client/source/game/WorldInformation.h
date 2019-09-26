#pragma once

#include <Client/source/resource/WorldResource.h>

#include <SFML/Graphics/Rect.hpp>

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

			info.defaultPlayerProperties.beginMovement = { 0.f, 0.f };
			info.defaultPlayerProperties.beginPosition = { 0.f, -1.f };

			info.defaultPlayerProperties.friction = 1.f;
			info.defaultPlayerProperties.speed = 1.f;
			info.defaultPlayerProperties.weight = 1.f;

			info.defaultWorldProperties.airResistance = 1.f;
			info.defaultWorldProperties.gravity = { 0.f, 1.f };

			info.defaultPlayerProperties.viewFollow = true;
			info.defaultPlayerProperties.viewWindow = { 0.f, 0.f, 20.f, 20.f };
			info.defaultPlayerProperties.viewRotation = 0.f;

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
			sf::Vector2f beginPosition;
			sf::Vector2f beginMovement;

			float speed, weight, friction;

			bool viewFollow;
			sf::FloatRect viewWindow;
			float viewRotation;

		} defaultPlayerProperties;
	};
}
