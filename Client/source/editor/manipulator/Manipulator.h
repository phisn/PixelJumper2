#pragma once

#include <Client/source/editor/manipulator/Component.h>
#include <Client/source/editor/manipulator/Element.h>

#include <Client/source/editor/World.h>

namespace Editor
{
	/*
	class Cache
	{
	public:
		TileTemplate* tile;

		struct
		{
			sf::Vector2i
				size,
				offset;

		} selection;
	};

	class Operation
	{
	public:
		Operation(
			World* const world)
			:
			world(world)
		{
		}

		virtual void run() = 0;

	protected:
		World* const world;
	};

	class FillOperation
		:
		public Operation
	{
	public:
		FillOperation(
			World* const world,

			const sf::Vector2i size,
			const sf::Vector2i offset,
			TileTemplate* const tile)
			:
			Operation(world),
			size(size),
			offset(offset),
			tile(tile)
		{
		}

		void run() override
		{
			const int x_end = offset.x + size.x;
			const int y_end = offset.y + size.y;

			for (int x = offset.x; x < x_end; ++x)
				for (int y = offset.y; y < y_end; ++y)
				{
					world->addTile(
						tile->create( sf::Vector2f(x, y) )
					);
				}
		}
		
	private:
		sf::Vector2f size, offset;
		TileTemplate* const tile;
	};

	enum class Mode
	{
		Default
	};

	class Executor
	{
	public:
		Executor(
			Cache* const cache,
			World* const world)
		{
		}

	private:
		void onSelectionChanged(
			const Cache::Selection* selection)
		{
			switch (mode)
			{

			}
		}

		Mode mode;
	};
	*/

	class Cache;
	class CacheManager;
	class Executor;

	namespace Manipulator
	{
		void initialize(
			World* const world);
		void uninitialize();

		Cache* getCache();
		CacheManager* getChaceManager();
		Executor* getExecutor();
	}
}
