#pragma once

#include <Client/source/editor/manipulator/Component.h>
#include <Client/source/editor/manipulator/Element.h>

#include <Client/source/game/tiles/TileId.h>

namespace Editor
{
	class CacheManager;
	class Executor;
	class World;

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
