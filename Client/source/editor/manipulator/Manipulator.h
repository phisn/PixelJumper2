#pragma once

#include <Client/source/editor/manipulator/Component.h>
#include <Client/source/editor/manipulator/Element.h>

#include <Client/source/editor/World.h>

namespace Editor
{
	class Cache;
	class CacheManager;
	class Executor;
}

namespace
{
	extern Editor::Cache* cache;
	extern Editor::CacheManager* manager;
	extern Editor::Executor* executor;
}

namespace Editor
{
	namespace Manipulator
	{
		void initialize(
			World* const world);
		void uninitialize();

		Cache* getCache()
		{
			return cache;
		}

		CacheManager* getCacheManager()
		{
			return manager;
		}

		Executor* getExecutor()
		{
			return executor;
		}
	}
}
