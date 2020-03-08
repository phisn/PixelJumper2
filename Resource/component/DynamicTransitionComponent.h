#pragma once

#include "ComponentResource.h"
#include "WorldResource.h"

#include "TrivialResource.h"

namespace Resource
{
	typedef sf::Uint32 WorldEntryID;

	struct DynamicWorldTransitionComponentContent
	{
		WorldEntryID worldEntryID;
		Resource::WorldId targetWorld;
		WorldEntryID targetEntry;
	};

	typedef TrivialResource<DynamicWorldTransitionComponentContent> DynamicWorldTransitionComponent;
}
