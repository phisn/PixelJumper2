#pragma once

#include "GameEvent.h"

namespace Game
{
	class ExitableTile
	{
	public:
		GameEvent<ExitableTile> onExit;

	protected:
		void notifyExitEvent()
		{
			onExit.notify();
		}
	};
}
