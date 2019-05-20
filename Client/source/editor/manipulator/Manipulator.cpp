#include "Manipulator.h"

#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Executor.h>

#include <Client/source/editor/manipulator/EditorWorld.h>
#include <Client/source/editor/template/TileTemplateFactory.h>

namespace
{
	Editor::Cache* cache = NULL;
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
		executor = new Executor(world);
	}

	void Manipulator::Uninitialize()
	{
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

	Executor* Manipulator::GetExecutor()
	{
		return executor;
	}

	const World* Manipulator::GetWorld()
	{
		return world;
	}
}
