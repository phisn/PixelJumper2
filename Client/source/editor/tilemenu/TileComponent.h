#pragma once

#include <Client/source/editor/manipulator/Component.h>
#include <Client/source/editor/tilemenu/TileMenuButton.h>

#include <Client/source/menu/container/ScrollContainer.h>
#include <Client/source/menu/style/DefaultStyles.h>

namespace Editor
{
	class TileComponent
		:
		public Component
	{
		typedef
			MENU::ScrollContainer<
				MENU::DefaultStyle::ScrollBar<MENU::ScrollBarBase>,
				MENU::DefaultStyle::RowContainer<MENU::RowContainerBase>
			> Container;
	public:
		void addTileButton(
			TileMenuButton* const button)
		{
			if (selected == NULL)
			{
				selected = button;
			}

			menuButtons.push_back(button);
		}

		void notify(
			const Cache::Sector sector)
		{
			if (sector == Cache::Sector::Tile)
			{
				onNotifyTile();
			}
		}

	private:
		TileMenuButton* selected = NULL;
		std::vector<TileMenuButton*> menuButtons;

		void onNotifyTile()
		{
			for (TileMenuButton* const button : menuButtons)
				if (button->getTemplate() == Manipulator::getCache()->writeInput()->tile.tile &&
					button != selected)
				{
					selected->unselect();
					selected = button;
				}
		}
	};
}
