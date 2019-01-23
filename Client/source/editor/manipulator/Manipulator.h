#pragma once

#include <Client/source/editor/manipulator/Component.h>
#include <Client/source/editor/manipulator/Element.h>

#include <Client/source/editor/World.h>


namespace Editor
{
	class CacheManager;
	class Executor;

	namespace Manipulator
	{
		void initialize(World* const world);
		void uninitialize();

		Cache* getCache();
		CacheManager* getCacheManager();
		Executor* getExecutor();
		const World* getWorld();
	}
}
