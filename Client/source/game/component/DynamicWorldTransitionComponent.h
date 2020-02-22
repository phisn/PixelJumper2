#pragma once

#include <Client/source/game/tiletrait/DynamicWorldEntry.h>
#include <Client/source/game/tiletrait/DynamicWorldExit.h>

#include <Client/source/game/tiletrait/TileContent.h>

namespace Game
{
	// duplex
	class DynamicWorldTransitionComponent
		:
		public DynamicWorldEntry,
		public DynamicWorldExit
	{
	public:
		DynamicWorldTransitionComponent(
			const WorldEntryID worldEntryID,
			const Resource::WorldId targetWorld, 
			const WorldEntryID targetEntry,
			const TileContent tilecontent)
			:
			DynamicWorldEntry(worldEntryID),
			targetWorld(targetWorld),
			targetEntry(targetEntry),
			tilecontent(tilecontent)
		{
		}

		void handleWorldEntry(
			PlayerBase* const player,
			const DynamicWorldEntryEvent& dwee) override
		{
			player->getProperties().position += tilecontent.position - dwee.offsetSource;
		}

	protected:
		void notifyOnExit()
		{
			DynamicWorldExitEvent event;

			event.offset = tilecontent.position;
			event.targetWorld = targetWorld;
			event.targetEntry = targetEntry;
			
			DynamicWorldExit::notifyOnExit(event);
		}

	private:
		const Resource::WorldId targetWorld;
		const WorldEntryID targetEntry;
		const TileContent tilecontent;
	};
}
