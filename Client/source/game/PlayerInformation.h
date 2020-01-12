#pragma once

#include <Client/source/resource/PlayerResource.h>

#include <SFML/Graphics.hpp>

#include <string>

namespace Game
{
	struct PlayerInformation
	{
		static PlayerInformation Create(const Resource::PlayerResource* const resource)
		{
			PlayerInformation information;

			information.playerId = resource->connection.HeaderIntro.playerId;
			information.name = resource->connection.HeaderAuth.userName;

			information.color = sf::Color::Red;

			return information;
		}

		Resource::PlayerId playerId;
		sf::Color color;
		std::wstring name;
	};
}
