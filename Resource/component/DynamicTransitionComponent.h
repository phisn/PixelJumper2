#pragma once

#include "Resource/ComponentResource.h"
#include "Resource/WorldResource.h"

#include "Resource/TrivialResource.h"

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
