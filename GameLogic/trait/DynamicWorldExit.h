#pragma once

#include "GameEvent.h"
#include "DynamicWorldEntry.h"

namespace Game
{
	struct DynamicWorldExitEvent
	{
		sf::Vector2f offset;
		Resource::WorldId targetWorld;
		WorldEntryID targetEntry;
	};

	class DynamicWorldExit
	{
	public:
		GameEvent<DynamicWorldExit, const DynamicWorldExitEvent&> onExit;
		
	protected:
		void notifyOnExit(const DynamicWorldExitEvent& event)
		{
			onExit.notify(event);
		}
	};
}
