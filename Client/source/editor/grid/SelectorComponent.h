#pragma once

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
			selection(&Manipulator::GetCache()->readOutput()->selection)
		{
			marker.setFillColor(sf::Color::Transparent);

			marker.setOutlineColor(markerColor);
			marker.setOutlineThickness(2.f);
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

		void drawSelection()
		{
			for (sf::RectangleShape& shape : selections)
			{
				Device::Screen::Draw(shape);
			}
		}

		void drawMarker()
		{
			Device::Screen::Draw(marker);
		}

	private:
		const SelectionCache::Output* const selection;

		void onSelection()
		{
			if (selection->type == SelectionCache::Output::Type::Area)
			{
				if (selection->typeChanged)
				{
					selections.clear();
					selections.emplace_back();

					selections.back().setFillColor(tileSelectionColor);
				}

				selections.back().setSize(sf::Vector2f(
					(float) selection->area->size.x,
					(float) selection->area->size.y
				));
				selections.back().setPosition(
					(float) selection->area->offset.x,
					(float) selection->area->offset.y);
			}
			else // == SelectionCache::Output::Type::Tile
			{
				selections.clear();

				for (TileBase* const tile : selection->tile->tiles)
				{
					selections.emplace_back();
					selections.back().setFillColor(tile->getShape().getFillColor() - areaSelectionColor);
					selections.back().setSize(
						tile->getShape().getSize()
					);
					selections.back().setPosition(
						tile->getShape().getPosition()
					);
				}
			}

			marker.setSize(
				selection->size);
			marker.setPosition(
				selection->offset);
		}

		sf::RectangleShape marker;
		std::vector<
			sf::RectangleShape> selections;

		const sf::Color areaSelectionColor = sf::Color(50, 50, 50, 0);
		const sf::Color tileSelectionColor = sf::Color(180, 180, 180, 100);
		const sf::Color markerColor = sf::Color(50, 50, 50, 100);
	};
}
