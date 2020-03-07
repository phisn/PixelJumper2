#pragma once

#include "Environment.h"

#include "tiletrait/DynamicWorldEntry.h"
#include "tiletrait/DynamicWorldExit.h"

#include "tiletrait/TileContent.h"

#include "Resource/component/DynamicWorldTransitionComponent.h"

namespace Game
{
	typedef Resource::DynamicWorldTransitionComponentContent DynamicWorldTransitionContent;

	// duplex
	class DynamicWorldTransitionComponent
		:
		public DynamicWorldEntry,
		public DynamicWorldExit
	{
	public:
		DynamicWorldTransitionComponent(
			const DynamicWorldTransitionContent content,
			const TileContent& tilecontent)
			:
			DynamicWorldEntry(content.worldEntryID),
			targetWorld(content.targetWorld),
			targetEntry(content.targetEntry),
			tilecontent(tilecontent)
		{
		}

		void handleWorldEntry(
			PlayerBase* const player,
			const DynamicWorldEntryEvent& dwee) override
		{
			player->getProperties().position += tilecontent.position - dwee.offsetSource;
		}

		void notifyOnExit()
		{
			DynamicWorldExitEvent event;

			event.offset = tilecontent.position;
			event.targetWorld = targetWorld;
			event.targetEntry = targetEntry;
			
			DynamicWorldExit::notifyOnExit(event);
		}

		void registerComponent(Environment* const env)
		{
			env->registerTile<DynamicWorldEntry>(this);
			env->registerTile<DynamicWorldExit>(this);
		}

	private:
		const Resource::WorldId targetWorld;
		const WorldEntryID targetEntry;
		const TileContent& tilecontent;
	};
}
