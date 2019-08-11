#pragma once

#include <Client/source/editor/selector/SelectorView.h>

#include <Client/source/editor/manipulator/EditorCache.h>
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
			const SelectorOutput* const selectorCache = Manipulator::GetCache()->selection.readOutput();
			const TileSearchOutput* const searchCache = Manipulator::GetCache()->tileSearch.readOutput();
			const TileChoiceOutput* const choiceCache = Manipulator::GetCache()->tileChoice.readOutput();

			if (choiceCache->selection == NULL)
			{
				Log::Error(L"Failed to place tile, no tile selected");

				return false;
			}

			if (selectorCache->type == SelectorType::Area)
			{
				sf::Vector2f begin =
				{
					selectorCache->area.left / SelectorView::GridRectSize,
					selectorCache->area.top / SelectorView::GridRectSize
				};

				sf::Vector2f end =
				{
					selectorCache->area.left / SelectorView::GridRectSize + selectorCache->area.width / SelectorView::GridRectSize,
					selectorCache->area.top / SelectorView::GridRectSize + selectorCache->area.height / SelectorView::GridRectSize
				};

				const sf::Uint64 expectedTiles = (sf::Uint64) abs(begin.x - end.x) * (sf::Uint64) abs(begin.y - end.y);
				if (expectedTiles > 0xff)
				{
					return false;
				}

				for (TilePosition x = begin.x; x < end.x; ++x)
					for (TilePosition y = begin.y; y < end.y; ++y)
					{
						TileBase* const newTile = choiceCache->selection->create(
							sf::Vector2f(x, y)
						);

						world->setTileSafe(newTile);
						placedTiles.push_back(newTile);
					}
			}
			else // Type::Tile
			{
				for (TileBase* const tile : selectorCache->tiles)
				{
					removedTiles.push_back(tile);

					if (tile->getTileId() == choiceCache->selection->getId())
					{
						world->removeTile(tile);
					}
					else
					{
						TileBase* const newTile = choiceCache->selection->create(
							tile->getPosition()
						);

						world->setTileUnsafe(newTile);
						placedTiles.push_back(newTile);
					}
				}
			}

			Manipulator::GetCache()->selection.notify();
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
			Manipulator::GetCache()->selection.notify();
		}

	private:
		std::vector<TileBase*> placedTiles;
		std::vector<TileBase*> removedTiles;
	};
}
