#include "Manipulator.h"

#include <Client/source/editor/manipulator/cache/Cache.h>
#include <Client/source/editor/manipulator/cache/CacheManager.h>
#include <Client/source/editor/manipulator/Executor.h>

#include <Client/source/editor/EditorWorld.h>
#include <Client/source/editor/template/WallTemplate.h>

#include <map>

namespace
{
	Editor::Cache* cache = NULL;
	Editor::CacheManager* manager = NULL;
	Editor::Executor* executor = NULL;
	Editor::World* world = NULL;

	std::map<Game::Tile::Id, Editor::TileTemplate*> tileTemplates;
}

#define EMPLACE_TILE(tile) tileTemplates.emplace(Game::Tile::Id:: ## tile, new Editor:: ## tile ## Template())

namespace Editor
{
	void Manipulator::Initialize()
	{
		world = new World();

		cache = new Cache();
		manager = new CacheManager(cache);
		executor = new Executor(world);

		// TILE_MARKER: extendable
		EMPLACE_TILE(Wall);
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

		for (const std::pair<Game::Tile::Id, TileTemplate*>& tileTemplate : tileTemplates)
		{
			delete tileTemplate.second;
		}

		tileTemplates.clear();
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

	TileTemplate* Manipulator::GetTemplate(
		const Game::Tile::Id tileId)
	{
		return tileTemplates[tileId];
	}
}
