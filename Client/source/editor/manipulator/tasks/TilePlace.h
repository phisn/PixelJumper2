#pragma once

#include <Client/source/editor/grid/GridProperties.h>

#include <Client/source/editor/manipulator/cache/Cache.h>
#include <Client/source/editor/manipulator/cache/CacheManager.h>
#include <Client/source/editor/manipulator/Executor.h>
#include <Client/source/editor/manipulator/Manipulator.h>

namespace Editor
{
	class TilePlace
		:
		public Task
	{
	public:
		bool execute(World* const world) override
		{
			const Cache::Output* const cache = Manipulator::GetCache()->readOutput();

			if (cache->tile.tile == NULL)
			{
				return false;
			}

			if (cache->selection.type == SelectionCache::Output::Type::Area)
			{
				VectorTilePosition begin =
				{
					cache->selection.area->offset.x / GridProperties.tileSize,
					cache->selection.area->offset.y / GridProperties.tileSize
				};

				VectorTilePosition end =
				{
					cache->selection.area->offset.x / GridProperties.tileSize + cache->selection.area->size.x / GridProperties.tileSize,
					cache->selection.area->offset.y / GridProperties.tileSize + cache->selection.area->size.y / GridProperties.tileSize
				};

				const sf::Uint64 expectedTiles = abs(begin.x - end.x) * abs(begin.y - end.y);
				if (expectedTiles > 0xff)
				{
					return false;
				}

				for (TilePosition x = begin.x; x < end.x; ++x)
					for (TilePosition y = begin.y; y < end.y; ++y)
					{
						TileBase* const newTile = cache->tile.tile->create(
							VectorTilePosition(x, y)
						);

						world->setTileSafe(newTile);
						placedTiles.push_back(newTile);
					}
			}
			else // Type::Tile
			{
				for (TileBase* const tile : cache->selection.tile->tiles)
				{
					removedTiles.push_back(tile);

					if (tile->id == cache->tile.tile->id)
					{
						world->removeTile(tile);
					}
					else
					{
						TileBase* const newTile = cache->tile.tile->create(
							tile->getPosition()
						);

						world->setTileUnsafe(newTile);
						placedTiles.push_back(newTile);
					}
				}
			}

			Manipulator::GetCacheManager()->notifyAll(Cache::Sector::Selection);
			return true;
		}

		void redo(World* const world) override
		{
			undo(world);
		}

		void undo(World* const world) override
		{
			for (TileBase* placedTile : placedTiles)
			{
				world->removeTile(placedTile);
			}

			for (TileBase* removedTile : removedTiles)
			{
				world->setTileUnsafe(removedTile);
			}

			placedTiles.swap(removedTiles);
			Manipulator::GetCacheManager()->notifyAll(Cache::Sector::Selection);
		}

	private:
		std::vector<TileBase*> placedTiles;
		std::vector<TileBase*> removedTiles;
	};
}
