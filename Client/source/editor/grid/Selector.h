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
			const World* const world,
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
			DEVICE::Interface::getScreen()->resetView();

			component.drawMarker();

			view->applyView();

			component.drawSelection();
		}

		void begin(
			const sf::Vector2f position)
		{
			offset = position;
		}

		void move(
			const sf::Vector2f position)
		{
			if (!active)
			{
				active = true;
			}

			SelectionCache::Input* const selection = 
				&Manipulator::getCache()->writeInput()->selection;

			selection->offset = offset;
			selection->size = position - offset;

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
		sf::Vector2f
			size,
			offset;
	};
}
