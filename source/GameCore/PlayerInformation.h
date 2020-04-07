#pragma once

#include "ResourceCore/PlayerResource.h"

#include <SFML/Graphics.hpp>

#include <string>

namespace Game
{
	struct PlayerInformation
	{
		static PlayerInformation Create(const Resource::PlayerResource* const resource)
		{
			PlayerInformation information;

			information.playerId = resource->content.playerID;
			information.name = resource->username;

			return information;
		}

		PlayerInformation(
			const Resource::PlayerID playerId,
			const Resource::RepresentationID representationID,
			const std::string name)
			:
			playerId(playerId),
			representationID(representationID),
			name(name)
		{
		}

		PlayerInformation()
			:
			playerId(0),
			representationID(0)
		{
		}

		Resource::PlayerID playerId;
		Resource::RepresentationID representationID;
		std::string name;
	};
}
