#pragma once

#include <Client/source/game/GamePlayer.h>

namespace Game
{
	typedef sf::Uint32 WorldEntryID;

	struct DynamicWorldEntryEvent
	{
		sf::Vector2f offsetSource;
	};

	class DynamicWorldEntry
	{
	public:
		DynamicWorldEntry(const WorldEntryID id)
			:
			id(id)
		{
		}

		virtual void handleWorldEntry(
			PlayerBase* const player,
			const DynamicWorldEntryEvent& dwee) = 0;

		const WorldEntryID id;
	};
}
