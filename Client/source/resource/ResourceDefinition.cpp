#include "ResourceDefinition.h"

#define CREATE_DEFINITION(name) \
	Resource::Definition( \
		Resource::ResourceTypeMagic::##name, \
		Resource::ResourceTypeExtension::HAS_##name, \
		Resource::ResourceTypeExtension::##name, \
		Resource::ResourceTypeName::##name,  \
		Resource::ResourceTypePath::##name) 
#define CHECK_DEFINITION_POSITION(name, position) \
	static_assert((int) Resource::ResourceType::##name == position, \
		"Resource '" #name "' on wrong type position")

namespace
{
	Resource::Definition definitions[(int) Resource::ResourceType::_Length] =
	{
		CREATE_DEFINITION(SETTINGS),
		CREATE_DEFINITION(WORLD),
		CREATE_DEFINITION(PLAYER)
	};

	CHECK_DEFINITION_POSITION(Settings, 0);
	CHECK_DEFINITION_POSITION(World, 1);
	CHECK_DEFINITION_POSITION(Player, 2);
}

namespace Resource
{
	const Definition* Resource::Definition::Get(
		const ResourceType type)
	{
		return &definitions[(int)type];
	}
}
