#pragma once

#include <Client/source/editor/grid/GridView.h>
#include <Client/source/editor/grid/SelectorComponent.h>
#include <Client/source/editor/grid/SelectorElement.h>

#include <Client/source/editor/manipulator/Cache.h>
#include <Client/source/editor/manipulator/CacheManager.h>
#include <Client/source/editor/manipulator/Manipulator.h>

namespace Editor
{
	/*
	
		Selector -> Core
		=	Component -> Drawing
		=	Element -> Handling transformation
	
	*/

	class Selector
	{
	public:
		Selector(
			GridView* const view)
			:
			view(view)
		{
		}

		void initialize()
		{
			Manipulator::getCacheManager()->registerComponent(
				Cache::Sector::Selection,
				);
		}

		void begin(
			const sf::Vector2f position)
		{
			offset = position;
		}

		void move(
			const sf::Vector2f position)
		{
			size = position - offset;
			
			Cache::Input::Selection* const selection = &Manipulator::getCache()->writeInput()->selection;

			/*
			const sf::Vector2f
				c_position1 = view->convertPtoC(sf::Vector2i(
					offset.x + (size.x < 0 ? size.x : 0),
					offset.y + (size.y < 0 ? size.y : 0)
				)), // 2 is always bigger
				c_position2 = view->convertPtoC(sf::Vector2i(
					offset.x + (size.x >= 0 ? size.x : 0),
					offset.y + (size.y >= 0 ? size.y : 0)
				));

			selection->altSelectionRect.setPosition();

			const sf::Vector2f c_real_position2 = sf::Vector2f(
				c_position2.x - fmod(c_position2.x, (float)view->getGridSize()) - (c_position2.x < 0 ? (float)view->getGridSize() : 0) + (float)view->getGridSize(),
				c_position2.y - fmod(c_position2.y, (float)view->getGridSize()) - (c_position2.y < 0 ? (float)view->getGridSize() : 0) + (float)view->getGridSize()
			);

			selection->altSelectionRect.setSize(sf::Vector2f(
				c_real_position2.x - selection->altSelectionRect.getPosition().x,
				c_real_position2.y - selection->altSelectionRect.getPosition().y
			));

			selection->tile_offset = sf::Vector2f(
				c_position1.x - fmod(c_position1.x, (float) view->getGridSize()) - (c_position1.x < 0 ? (float) view->getGridSize() : 0),
				c_position1.y - fmod(c_position1.y, (float) view->getGridSize()) - (c_position1.y < 0 ? (float) view->getGridSize() : 0)
			);

			Manipulator::g
			*/
		}

	private:
		GridView* view;

		SelectorComponent component;
		SelectorElement element;

		sf::Vector2f
			size,
			offset;
	};
}
