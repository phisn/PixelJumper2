#pragma once

#include <Client/source/resource/ResourceBase.h>
#include <Client/source/resource/WorldResource.h>

namespace Resource
{
	const Resource::Definition resourceDefinitions[(int)Resource::ResourceType::_Length]
	{
		Resource::Definition(
			0x0000'0000,
			L"World",
			[]() -> Resource::Base* 
		{
			return new Resource::World();
		})
	};
}
