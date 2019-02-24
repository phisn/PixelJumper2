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
		CREATE_DEFINITION(STATIC),
		CREATE_DEFINITION(DYNAMIC),
		CREATE_DEFINITION(WORLD),
		CREATE_DEFINITION(PLAYER),
	};

	CHECK_DEFINITION_POSITION(Static, 0);
	CHECK_DEFINITION_POSITION(Dynamic, 1);
	CHECK_DEFINITION_POSITION(World, 2);
	CHECK_DEFINITION_POSITION(Player, 3);
}

namespace Resource
{
	const Definition* Resource::Definition::Get(
		const ResourceType type)
	{
		return &definitions[(int)type];
	}
}
