#pragma once

#include "PlayerResource.h"

namespace Resource
{
	// used between operator, simulator and client
	// created by operator from database
	// usally not saved, maybe for caching but questionable
	class ClassicPlayerResource
		:
		public ResourceBase
	{
	public:
		std::vector<RepresentationID> unlockedRepresentations;
		std::vector<Resource::WorldId> unlockedWorlds;

		bool make(ReadPipe* const pipe) override
		{
			return pipe->readVector(&unlockedRepresentations)
				&& pipe->readVector(&unlockedWorlds);
		}

		bool save(WritePipe* const pipe) override
		{
			return pipe->writeVector(&unlockedRepresentations)
				&& pipe->writeVector(&unlockedWorlds);
		}

		bool setup() override
		{
			return unlockedRepresentations.size() > 0
				&& unlockedWorlds.size() > 0;
		}

		bool validate() override
		{
			return unlockedRepresentations.size() > 0
				&& unlockedWorlds.size() > 0;
		}
	};
}
