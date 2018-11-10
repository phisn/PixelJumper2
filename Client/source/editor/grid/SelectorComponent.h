#pragma once

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

	private:
		void onSelection()
		{
			const SelectionCache::Output* const selection = 
				&Manipulator::getCache()->readOutput()->selection;

			if (selection->type == SelectionCache::Output::Type::Area)
			{
				selection-
			}
			else // == SelectionCache::Output::Type::Tile
			{

			}
		}
	};
}
