#pragma once

#include <Client/source/editor/grid/GridView.h>
#include <Client/source/editor/grid/SelectorComponent.h>
#include <Client/source/editor/grid/SelectorElement.h>

#include <Client/source/editor/manipulator/cache/Cache.h>
#include <Client/source/editor/manipulator/cache/CacheManager.h>
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
			const World_N* const world,
			GridView* const view)
			:
			view(view),
			element(view, world)
		{
			Manipulator::getCacheManager()->registerComponent(
				Cache::Sector::Selection,
				&component);
			Manipulator::getCacheManager()->registerElement(
				Cache::Sector::Selection,
				&element);
		}

		void draw()
		{
			DEVICE::Interface::GetScreen()->resetView();

			component.drawMarker();

			view->applyView();

			component.drawSelection();
		}

		void begin(
			const int x,
			const int y)
		{
			offset = sf::Vector2i(x, y);
		}

		void move(
			const int x,
			const int y)
		{
			if (!active)
			{
				active = true;
			}

			SelectionCache::Input* const selection = 
				&Manipulator::getCache()->writeInput()->selection;

			selection->offset = offset;
			selection->size = sf::Vector2i(x, y) - offset;

			Manipulator::getCacheManager()->notify(	
				Cache::Sector::Selection
			);
		}

		bool release()
		{
			if (active)
			{
				active = false;

				return false;
			}

			return true;
		}

	private:
		bool active = false;

		SelectorComponent component;
		SelectorElement element;

		GridView* const view;
		sf::Vector2i
			size,
			offset;
	};
}
