#pragma once

#include "ResourceCore/ComponentResource.h"
#include "ResourceCore/WorldResource.h"

#include "ResourceCore/TrivialResource.h"

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
