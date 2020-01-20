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

			information.playerId = resource->connection.content.playerId;
			information.name = resource->connection.content.userName;

			information.color = sf::Color::Red;

			return information;
		}

		PlayerInformation(
			const Resource::PlayerID playerId,
			const sf::Color color,
			const std::wstring name)
			:
			playerId(playerId),
			color(color),
			name(name)
		{
		}

		PlayerInformation()
			:
			playerId(0),
			color(sf::Color::White)
		{
		}

		Resource::PlayerID playerId;
		sf::Color color;
		std::wstring name;
	};
}
