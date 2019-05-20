#pragma once

#include <Client/source/editor/manipulator/EditorCache.h>

namespace Editor
{
	class Cache;
	class Executor;
	class World;

	namespace Manipulator
	{
		void Initialize();
		void Uninitialize();

		Cache* GetCache();
		Executor* GetExecutor();
		const World* GetWorld();
	}
}
