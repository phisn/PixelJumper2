#include "Manipulator.h"

#include <Client/source/editor/manipulator/cache/Cache.h>
#include <Client/source/editor/manipulator/cache/CacheManager.h>
#include <Client/source/editor/manipulator/Executor.h>

namespace
{
	Editor::Cache* cache = NULL;
	Editor::CacheManager* manager = NULL;
	Editor::Executor* executor = NULL;
}

namespace Editor
{
	void Manipulator::initialize(World* const world)
	{
		cache = new Cache();
		manager = new CacheManager(cache);
		executor = new Executor(world);
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

		if (manager)
		{
			delete manager;
		}
	}

	Cache* Manipulator::getCache()
	{
		return cache;
	}

	CacheManager* Manipulator::getCacheManager()
	{
		return manager;
	}

	Executor* Manipulator::getExecutor()
	{
		return executor;
	}
}
