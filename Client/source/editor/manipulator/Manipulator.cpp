#include "Manipulator.h"
#include <Client/source/editor/manipulator/Cache.h>
#include <Client/source/editor/manipulator/Executor.h>

namespace
{
	Editor::Cache* cache = NULL;
	Editor::CacheManager* executor = NULL;
}

namespace Editor
{
	void Manipulator::initialize(
		World* const world)
	{
		cache = new Cache();
		executor = new CacheManager(
			cache,
			world);
	}

	void Manipulator::uninitialize()
	{
		if (cache)
		{
			delete cache;
		}

		if (executor)
		{
			delete executor;
		}
	}

	Cache* Manipulator::getCache()
	{
		return cache;
	}
}
