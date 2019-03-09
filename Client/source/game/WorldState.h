#pragma once

#include <Client/source/game/GameState.h>
#include <Client/source/game/PlayerState.h>

#include <map>
#include <vector>

#pragma pack(push, 1)

namespace Game
{
	typedef sf::Uint16 TileStateId; // == index

	struct WorldStateProperties
	{
		// addition
		sf::Vector2f gravity = { 0.f, -1.f };

		float friction = 0.53f; // == 5% lose

		sf::Uint32 milisecondsPassed;
		sf::Uint16 playerCount;
	};

	class WorldState
		:
		public GameState
	{
		WorldStateProperties properties;
	public:
		bool writeState(
			Resource::WritePipe* const writePipe) override
		{
			return writePipe->writeValue(&properties);
		}

		bool readState(
			Resource::ReadPipe* const readPipe) override
		{
			return readPipe->readValue(&properties);
		}

		const WorldStateProperties* readProperties() const
		{
			return &properties;
		}

		PropertyWriter<sf::Vector2f> gravity{ properties.gravity };

		PropertyWriter<float> drag{ properties.friction };

		PropertyWriter<sf::Uint32> milisecondsPassed{ properties.milisecondsPassed };
		PropertyWriter<sf::Uint16> playerCount{ properties.playerCount };
	};
}

#pragma pack(pop)
