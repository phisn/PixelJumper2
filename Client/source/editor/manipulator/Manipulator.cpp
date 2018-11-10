#include "Manipulator.h"

#include <Client/source/editor/manipulator/Cache.h>
#include <Client/source/editor/manipulator/CacheManager.h>
#include <Client/source/editor/manipulator/Executor.h>

namespace
{
	Editor::Cache* cache = NULL;
	Editor::CacheManager* manager = NULL;
	Editor::Executor* executor = NULL;
}

namespace Editor
{
	void Manipulator::initialize(
		World* const world)
	{
		cache = new Cache();
		executor = new Executor();
		manager = new CacheManager(
			cache);
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
}
