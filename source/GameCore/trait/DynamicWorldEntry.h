#pragma once

#include "GamePlayer.h"

#include <unordered_map>

namespace Game
{
	typedef sf::Uint32 WorldEntryID;

	struct DynamicWorldEntryEvent
	{
		sf::Vector2f offsetSource;
	};

	struct DynamicWorldEntryHandler
	{
		virtual void handleWorldEntry(
			PlayerBase* const player,
			const DynamicWorldEntryEvent& dwee) = 0;
	};

	class DynamicWorldEntryContainer
	{
	public:
		void pushDynamicWorldEntry(
			DynamicWorldEntryHandler* const handler,
			const WorldEntryID entryID)
		{
			dynamicWorldEntries[entryID] = handler;
		}

		DynamicWorldEntryHandler* findDynamicWorldEntry(const WorldEntryID entryID) const
		{
			decltype(dynamicWorldEntries)::const_iterator entry = dynamicWorldEntries.find(entryID);

			if (entry == dynamicWorldEntries.end())
			{
				return NULL;
			}

			return entry->second;
		}

	protected:
		std::unordered_map<WorldEntryID, DynamicWorldEntryHandler*> dynamicWorldEntries;
	};
}
