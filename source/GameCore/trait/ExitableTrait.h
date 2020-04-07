#pragma once

#include "GameEvent.h"

namespace Game
{
	class ExitableTraitHandler
	{
	public:
		GameEvent<ExitableTraitHandler> onExit;

	protected:
		void notifyExitEvent()
		{
			onExit.notify();
		}
	};

	class ExitableTraitContainer
	{
	public:
		void pushExitableTraitTrait(ExitableTraitHandler* const handler)
		{
			exitableTraitHandlers.push_back(handler);
		}

		const std::vector<ExitableTraitHandler*>& getExitableTraitTrait() const
		{
			return exitableTraitHandlers;
		}

	protected:
		std::vector<ExitableTraitHandler*> exitableTraitHandlers;
	};
}
