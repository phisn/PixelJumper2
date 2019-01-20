#pragma once

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
			const Cache::Output* const cache = Manipulator::getCache()->readOutput();

			if (cache->selection.type == SelectionCache::Output::Type::Area)
			{
				for (TilePosition x = cache->selection.area->offset.x / 20
					; x < cache->selection.area->offset.x / 20
					+ cache->selection.area->size.x / 20
					; ++x)
					for (TilePosition y = cache->selection.area->offset.y / 20
						; y < cache->selection.area->offset.y / 20
						+ cache->selection.area->size.y / 20
						; ++y)
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
		}

		void redo() override
		{

		}

		void undo() override
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
