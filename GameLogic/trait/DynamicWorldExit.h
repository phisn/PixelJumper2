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

	class DynamicWorldExitHandler
	{
	public:
		GameEvent<DynamicWorldExitHandler, const DynamicWorldExitEvent&> onExit;

	protected:
		void notifyOnExit(const DynamicWorldExitEvent& event)
		{
			onExit.notify(event);
		}
	};

	class DynamicWorldExitContainer
	{
	public:
		void pushDynamicWorldExit(DynamicWorldExitHandler* const handler)
		{
			dynamicWorldExits.push_back(handler);
		}

		const std::vector<DynamicWorldExitHandler*>& getDynamicWorldExit() const
		{
			return dynamicWorldExits;
		}

	protected:
		std::vector<DynamicWorldExitHandler*> dynamicWorldExits;
	};
}
