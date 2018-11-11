#pragma once

#include <Client/source/editor/grid/GridView.h>
#include <Client/source/editor/manipulator/Element.h>
#include <Client/source/editor/World.h>

namespace Editor
{
	class SelectorElement
		:
		public Element
	{
	public:
		SelectorElement(
			const GridView* const view,
			const World* const world)
			:
			view(view),
			world(world)
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

			determineType();

			if (this->output->type == SelectionCache::Output::Type::Area)
			{
				createArea();
			}
		}
	private:
		SelectionCache::Input* input;
		SelectionCache::Output* output;

		void createArea()
		{
			const sf::Vector2f
				c_position1 = view->convertPtoC(sf::Vector2i(
					input->offset.x + (input->size.x < 0 ? input->size.x : 0),
					input->offset.y + (input->size.y < 0 ? input->size.y : 0)
				)), // 2 is always bigger
				c_position2 = view->convertPtoC(sf::Vector2i(
					input->offset.x + (input->size.x >= 0 ? input->size.x : 0),
					input->offset.y + (input->size.y >= 0 ? input->size.y : 0)
				));


			output->area->offset = sf::Vector2f(
				c_position1.x - fmod(c_position1.x, (float)view->getGridSize()) - (c_position1.x < 0 ? (float)view->getGridSize() : 0),
				c_position1.y - fmod(c_position1.y, (float)view->getGridSize()) - (c_position1.y < 0 ? (float)view->getGridSize() : 0)
			);

			const sf::Vector2f c_real_position2 = sf::Vector2f(
				c_position2.x - fmod(c_position2.x, (float)view->getGridSize()) - (c_position2.x < 0 ? (float)view->getGridSize() : 0) + (float)view->getGridSize(),
				c_position2.y - fmod(c_position2.y, (float)view->getGridSize()) - (c_position2.y < 0 ? (float)view->getGridSize() : 0) + (float)view->getGridSize()
			);

			output->area->size = c_real_position2 - output->area->offset;
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
			for (TileBase* tile : world->getTiles())
				if (tile->getShape()->getPosition().x + tile->getShape()->getSize().x > input->offset.x &&
					tile->getShape()->getPosition().y + tile->getShape()->getSize().y > input->offset.y &&

					tile->getShape()->getPosition().x < input->offset.x + input->size.x &&
					tile->getShape()->getPosition().y < input->offset.y + input->size.y)
				{
					if (!tileFound)
					{
						tileFound = true;

						changeType(
							SelectionCache::Output::Type::Tile);
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
		const World* const world;
	};
}
