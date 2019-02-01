#include "Manipulator.h"

#include <Client/source/editor/manipulator/cache/Cache.h>
#include <Client/source/editor/manipulator/cache/CacheManager.h>
#include <Client/source/editor/manipulator/Executor.h>

#include <Client/source/editor/EditorWorld.h>
#include <Client/source/editor/template/TileTemplateFactory.h>

namespace
{
	Editor::Cache* cache = NULL;
	Editor::CacheManager* manager = NULL;
	Editor::Executor* executor = NULL;
	Editor::World* world = NULL;
}

namespace Editor
{
	void Manipulator::Initialize()
	{
		TileTemplateFactory::Initialize();

		world = new World();

		cache = new Cache();
		manager = new CacheManager(cache);
		executor = new Executor(world);
	}

	void Manipulator::Uninitialize()
	{
		if (manager)
		{
			delete manager;
		}

		if (executor)
		{
			delete executor;
		}

		if (cache)
		{
			delete cache;
		}

		if (world)
		{
			delete world;
		}

		TileTemplateFactory::Uninitialize();
	}

	Cache* Manipulator::GetCache()
	{
		return cache;
	}

	CacheManager* Manipulator::GetCacheManager()
	{
		return manager;
	}

	Executor* Manipulator::GetExecutor()
	{
		return executor;
	}

	const World* Manipulator::GetWorld()
	{
		return world;
	}
}
