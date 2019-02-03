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
	public:
		sf::Uint32 milisecondsPassed;
		sf::Uint16 playerCount;

	};
}

#pragma pack(pop)
