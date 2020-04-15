#pragma once

#include "GameEvent.h"

namespace Game
{
	struct ExitWorldEvent
	{
		Resource::WorldID worldID;
	};

	class ExitableTraitHandler
	{
	public:
		GameEvent<ExitableTraitHandler, const ExitWorldEvent&> onExit;

	protected:
		void notifyExitEvent(const ExitWorldEvent& event)
		{
			onExit.notify(event);
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
