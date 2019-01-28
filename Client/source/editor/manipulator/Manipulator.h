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
		void Initialize();
		void Uninitialize();

		Cache* GetCache();
		CacheManager* GetCacheManager();
		Executor* GetExecutor();
		const World* GetWorld();
	}
}
