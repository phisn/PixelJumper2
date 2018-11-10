#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/editor/manipulator/cache/Cache.h>
#include <Client/source/editor/manipulator/Component.h>
#include <Client/source/editor/manipulator/Manipulator.h>

namespace Editor
{
	class SelectorComponent
		:
		public Component
	{
	public:
		SelectorComponent()
			:
			selection(&Manipulator::getCache()->readOutput()->selection)
		{
		}

		void notify(
			const Cache::Sector sector) override
		{
			switch (sector)
			{
			case Cache::Sector::Selection:
				onSelection();

				break;
			}
		}

		void draw()
		{
			for (sf::RectangleShape& shape : shapes)
			{
				DEVICE::Interface::getScreen()->onDraw(
					&shape);
			}
		}

	private:
		const SelectionCache::Output* const selection;

		void onSelection()
		{
			if (selection->type == SelectionCache::Output::Type::Area)
			{
				if (selection->typeChanged)
				{
					shapes.clear();
					shapes.emplace_back();

					shapes.back().setFillColor(selectionColor);
				}

				shapes.back().setSize(sf::Vector2f(
					selection->area->size.x,
					selection->area->size.y
				));
				shapes.back().setPosition(
					selection->area->offset.x,
					selection->area->offset.y);
			}
			else // == SelectionCache::Output::Type::Tile
			{
				shapes.clear();

				for (TileBase* const tile : selection->tile->tiles)
				{
					shapes.emplace_back();

					shapes.back().setFillColor(selectionColor);
					shapes.back().setSize(
						tile->getShape()->getSize()
					);
					shapes.back().setPosition(
						tile->getShape()->getPosition()
					);
				}
			}
		}

		std::vector<
			sf::RectangleShape> shapes;

		const sf::Color selectionColor =
			sf::Color(150, 150, 150, 100);
	};
}
