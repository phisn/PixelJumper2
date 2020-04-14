#pragma once

#include "ComponentResource.h"
#include "ResourceBase.h"

namespace Resource
{
	struct EntityResource
		:
		public ResourceBase
	{
		std::vector<sf::Uint16> componentIndex;

		bool make(ReadPipe* const pipe) override
		{
			return pipe->readVector(&componentIndex);
		}

		bool save(WritePipe* const pipe) override
		{
			return pipe->writeVector(&componentIndex);
		}
	};
}
