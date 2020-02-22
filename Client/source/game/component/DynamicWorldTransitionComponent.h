#pragma once

#include <Client/source/game/tiletrait/DynamicWorldEntry.h>
#include <Client/source/game/tiletrait/DynamicWorldExit.h>

#include <Client/source/game/tiletrait/TileContent.h>

namespace Game
{
	class DynamicWorldTransitionComponent
		:
		public DynamicWorldEntry,
		public DynamicWorldExit
	{
	public:
		DynamicWorldTransitionComponent(
			const WorldEntryID worldEntryID,
			const TileContent tilecontent)
			:
			DynamicWorldEntry(worldEntryID),
			tilecontent(tilecontent)
		{
		}

		void handleWorldEntry(
			PlayerBase* const player,
			const DynamicWorldEntryEvent& dwee) override
		{
			player->getProperties().position = *player->getProperties().position + dwee.offsetSource - tilecontent.position;
		}

	private:
		const TileContent tilecontent;
	};
}
