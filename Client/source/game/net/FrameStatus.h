#pragma once

#include <Client/source/game/PlayerProperties.h>
#include <Client/source/game/WorldProperties.h>

#include <Client/source/device/InputDevice.h>

namespace Game
{
	/*
	
	Non interactive multiplayer:
	-	Players do not interact with other players worlds.
	-	The Server does simulate a independent world for
		each single player
	-	Each simulation does only handle his own player
		requests and syncs
	-	Each simulation does wait with the world tickrate
		for player input (no input is also an input)
	->	If a simulation does not get the input for a frame
		he can request it specifically or call a timeout.
	-->	Alternative would be to skip a frame (empty input)
	->	Every N ticks a sync is forced onto the player.
	-	A player cannot sync a simulation, a simulation
		must sync a player
	-	Other players from other simulation can get the
		player state to show them as ghosts. These are
		usally behind realtime because the simulation waits
		for each player individually

	Interactive multiplayer: (similar to non interactive multiplayer)
	-	Players can interact with other players worlds
	-	The server does simulate a shared world for a group
		of players
	-	The simulation does wait for all player inputs
		before finishing a tick
	-	If a players input does need to long, it is ignored
		and counted as empty
	-	If a player does have too many missing inputs in
		a row, a timeout is called

	*/

	namespace Net
	{
		// clients request / send framestatus to server
		// to give input informations in each frame
		// -> only server has valid information in all time
		class FrameStatus
			:
			public GameState
		{
		public:
			void setKey(
				const Device::GameCoreInputSymbol key,
				const bool state)
			{
				keyStates[(int) key] = state;
			}

			bool getKey(const Device::GameCoreInputSymbol key) const
			{
				return keyStates[(int) key];
			}

			bool writeState(Resource::WritePipe* const writePipe)
			{
				char buffer;

				for (int i = 0; i < (int) Device::GameCoreInputSymbol::_Length / 8 + 1; ++i)
				{
					buffer = 0;

					for (int j = 0; j < 8; ++j)
					{
						buffer |= keyStates[i] << j;
					}

					if (!writePipe->writeValue(&buffer))
					{
						return false;
					}
				}

				return true;
			}

			bool readState(Resource::ReadPipe* const readPipe)
			{
				char buffer;

				for (int i = 0; i < (int)Device::GameCoreInputSymbol::_Length / 8 + 1; ++i)
				{
					if (!readPipe->readValue(&buffer))
					{
						return false;
					}

					for (int j = 8 - 1; j > 0 - 1; --j)
					{
						keyStates[i] = !!(buffer & 1 << j);
					}
				}

				return true;
			}

		private:
			bool keyStates[(int) Device::GameCoreInputSymbol::_Length];
		};
	}
}
