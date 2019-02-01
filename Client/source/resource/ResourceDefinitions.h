#pragma once

#include <Client/source/resource/ResourceBase.h>
#include <Client/source/resource/WorldResource.h>

namespace Resource
{
	const Resource::ResourceDefinition ResourceDefinitions[(int)Resource::ResourceType::_Length]
	{
		Resource::ResourceDefinition(
			0x0000'0000,
			L"World",
			[]() -> Resource::ResourceBase* 
		{
			return new Resource::World();
		})
	};
}
