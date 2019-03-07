#pragma once

#include <Client/source/game/Environment.h>
#include <Client/source/resource/WorldResource.h>

namespace Game
{
	class WorldBase
	{
	public:
		bool initialize(
			Resource::World* const resource)
		{
		}

	private:
		Environment envrionment;
	};
}
