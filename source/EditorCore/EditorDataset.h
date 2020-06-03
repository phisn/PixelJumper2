#pragma once

#include "EditorDatabase.h"

#include "ResourceCore/ClassicContextResource.h"
#include "ResourceCore/WorldResource.h"
#include "ResourceCore/component/DynamicTransitionComponent.h"

namespace Editor
{
	struct ContextDataset
	{
		ContextDataset(ContextTuple tuple)
			:
			contextID(std::get<ContextColumn::ID>(tuple)),
			name(std::get<ContextColumn::Name>(tuple)),
			description(std::get<ContextColumn::Description>(tuple))
		{
		}

		Resource::ContextID contextID;
		std::string name;
		std::string description;
	};

	struct ContextEvent
	{
		Resource::ContextID contextID;
	};

	struct WorldEvent
	{
		Resource::WorldID worldID;
		Resource::ContextID contextID;
	};

	struct TransitiveEvent
	{
		Resource::WorldEntryID entryID;
		Resource::WorldID outputWorldID;
		Resource::WorldID inputWorldID;
		Resource::ContextID contextID;
	};

	struct TileEvent
	{
		Resource::WorldID worldID;
	};
}
