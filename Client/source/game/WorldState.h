#pragma once

#include <Client/source/game/GameState.h>
#include <Client/source/game/PlayerState.h>

#include <map>
#include <vector>

#pragma pack(push, 1)

namespace Game
{
	typedef sf::Uint16 TileStateId; // == index

	class WorldState
		:
		public GameState
	{
		struct
		{
			// addition
			sf::Vector2f gravity = { 0.f, -1.f };
			
			float drag = 0.53f; // == 5% lose

			sf::Uint32 milisecondsPassed;
			sf::Uint16 playerCount;

		} Properties;

	public:
		StateProperty<sf::Vector2f> gravity{ Properties.gravity };

		StateProperty<float> drag{ Properties.drag };

		StateProperty<sf::Uint32> milisecondsPassed{ Properties.milisecondsPassed };
		StateProperty<sf::Uint16> playerCount{ Properties.playerCount };

	private:
		bool writeState(
			Resource::WritePipe* const writePipe) override
		{
			return writePipe->writeValue(&Properties);
		}

		bool readState(
			Resource::ReadPipe* const readPipe) override
		{
			return readPipe->readValue(&Properties);
		}
	};
}

#pragma pack(pop)
