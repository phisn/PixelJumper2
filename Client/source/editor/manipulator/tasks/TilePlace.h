#pragma once

#include <Client/source/editor/grid/GridProperties.h>

#include <Client/source/editor/manipulator/cache/Cache.h>
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
						world->setTileSafe(
							cache->tile.tile->create( VectorTilePosition(x, y) )
						);
					}

				return true;
			}
			else // Type::Tile
			{
				for (TileBase* const tile : cache->selection.tile->tiles)
				{
					world->replaceTileTemplate(tile, cache->tile.tile);
				}

				return true;
			}

			return false;
		}

		void redo(World* const world) override
		{

		}

		void undo(World* const world) override
		{

		}

	private:
		struct
		{
			TileTemplate* tile;


		} newContent;

		struct
		{


		} oldContent;
	};
}
