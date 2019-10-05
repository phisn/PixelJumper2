#pragma once

#include <Client/source/game/PlayerProperties.h>
#include <Client/source/game/WorldProperties.h>

#include <Client/source/device/InputDevice.h>

namespace Game
{
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
