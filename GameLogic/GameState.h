#pragma once

#include "Resource/pipes/PipeBase.h"

namespace Game
{
	struct GameState
	{
		virtual bool writeState(Resource::WritePipe* const writePipe) = 0;
		virtual bool readState(Resource::ReadPipe* const readPipe) = 0;
	};

	class LazyGameState
		:
		public GameState
	{
	public:
		virtual bool writeState(Resource::WritePipe* const writePipe) override
		{
			if (stateChanged)
			{
				stateChanged = false;
				
				if (!writePipe->writeValue(&stateChanged))
				{
					return false;
				}

				return writeChangedState(writePipe);
			}
		}

		virtual bool readState(Resource::ReadPipe* const readPipe) override
		{
			if (!readPipe->readValue(&stateChanged))
			{
				return false;
			}

			if (stateChanged)
			{
				return readChangedState(readPipe);
			}

			return true;
		}

		// does only affect write state
		void changeState()
		{
			stateChanged = true;
		}

		bool isStateChanged() const
		{
			return stateChanged;
		}

	private:
		virtual bool writeChangedState(Resource::WritePipe* const writePipe) = 0;
		virtual bool readChangedState(Resource::ReadPipe* const readPipe) = 0;

		bool stateChanged = false;
	};
}
