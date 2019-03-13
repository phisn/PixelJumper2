#pragma once

#include <Client/source/editor/grid/GridView.h>
#include <Client/source/editor/manipulator/Element.h>
#include <Client/source/editor/manipulator/Manipulator.h>
#include <Client/source/editor/EditorWorld.h>

namespace Editor
{
	class SelectorElement
		:
		public Element
	{
	public:
		SelectorElement(
			const GridView* const view)
			:
			view(view)
		{
		}

		bool manipulate(
			Cache::Input* const input,
			Cache::Output* const output,

			const Cache::Sector sector) override
		{
			if (sector != Cache::Sector::Selection)
			{
				return false;
			}

			this->input = &input->selection;
			this->output = &output->selection;

			convertRange();
			determineType();

			if (this->output->type == SelectionCache::Output::Type::Area)
			{
				createArea();
			}

			return true;
		}
	private:
		SelectionCache::Input* input;
		SelectionCache::Output* output;

		sf::Vector2f selectPosition1, selectPosition2;

		void convertRange()
		{
			selectPosition1 = view->mapPixelToCoords(sf::Vector2i(
				input->offset.x + (input->size.x < 0 ? input->size.x : 0),
				input->offset.y + (input->size.y < 0 ? input->size.y : 0)
			)); // 2 is always bigger
			selectPosition2 = view->mapPixelToCoords(sf::Vector2i(
				input->offset.x + (input->size.x >= 0 ? input->size.x : 0),
				input->offset.y + (input->size.y >= 0 ? input->size.y : 0)
			));
		}

		void createArea()
		{
			output->area->offset = VectorTilePosition(
				(TilePosition) (selectPosition1.x - fmod(selectPosition1.x, (float)view->getGridSize()) - (selectPosition1.x < 0 ? (float)view->getGridSize() : 0)),
				(TilePosition) (selectPosition1.y - fmod(selectPosition1.y, (float)view->getGridSize()) - (selectPosition1.y < 0 ? (float)view->getGridSize() : 0))
			);

			const VectorTilePosition c_real_position2 = VectorTilePosition(
				(TilePosition) (selectPosition2.x - fmod(selectPosition2.x, (float)view->getGridSize()) - (selectPosition2.x < 0 ? (float)view->getGridSize() : 0) + (float)view->getGridSize()),
				(TilePosition) (selectPosition2.y - fmod(selectPosition2.y, (float)view->getGridSize()) - (selectPosition2.y < 0 ? (float)view->getGridSize() : 0) + (float)view->getGridSize())
			);

			output->area->size = VectorTileSize( c_real_position2 - output->area->offset );
		}

		void determineType() // & load tile tiles
		{
			bool tileFound = false;

			/*
			
				Speedup:
				1.	Test if next selection entered new
					tile "zone". % tileSize

				2.  Add new tiles to existent tiles
				
			*/

			for (Editor::TileBase* tile : Manipulator::GetWorld()->getTiles())
				if (tile->getShape().getPosition().x + tile->getShape().getSize().x > selectPosition1.x &&
					tile->getShape().getPosition().y + tile->getShape().getSize().y > selectPosition1.y &&

					tile->getShape().getPosition().x < selectPosition2.x &&
					tile->getShape().getPosition().y < selectPosition2.y)
				{
					if (!tileFound)
					{
						tileFound = true;

						changeType(SelectionCache::Output::Type::Tile);
					}

					output->tile->tiles.push_back(tile);
				}

			if (!tileFound)
			{
				changeType(
					SelectionCache::Output::Type::Area);
			}
		}

		void changeType(
			const SelectionCache::Output::Type newType)
		{
			if (output->typeChanged = output->type != newType)
			{
				switch (output->type)
				{
				case SelectionCache::Output::Type::Area:
					delete output->area;

					break;
				case SelectionCache::Output::Type::Tile:
					delete output->tile;

					break;
				}

				switch (newType)
				{
				case SelectionCache::Output::Type::Area:
					output->area = new SelectionCache::Area();

					break;
				case SelectionCache::Output::Type::Tile:
					output->tile = new SelectionCache::Tile();

					break;
				}
			}
			else
			{
				switch (newType)
				{
				case SelectionCache::Output::Type::Tile:
					output->tile->tiles.clear();

					break;
				}
			}

			output->type = newType;
		}

		const GridView* const view;
	};
}
