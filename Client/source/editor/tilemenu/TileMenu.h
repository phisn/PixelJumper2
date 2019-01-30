#pragma once

#include <Client/source/menu/RootBase.h>

#include <Client/source/editor/tilemenu/TileComponent.h>
#include <Client/source/editor/tilemenu/TileContainer.h>

#include <Client/source/editor/template/WallTemplate.h>

namespace Editor
{
	/*
	
		Place tiles by (tileType = selected by tilemenu):
		-	Click with mouse on already
			selected  tile in  tilemenu
		-	Open with mouse contextmenu
			(inside  selected  area  in 
			gridmenu)
			and   select  "place tiles"
		-	Press  the   'Enter' button

	*/
	class TileMenu
		:
		public MENU::RootBase
	{
	public:
		TileMenu()
			:
			MENU::RootBase(),
			container( getView() )
		{
			useOnLogic = false;

			static class QElement
				:
				public Element
			{
			public:
				bool manipulate(
					Cache::Input* const input,
					Cache::Output* const output,

					const Cache::Sector sector) override
				{
					if (sector == Cache::Sector::Tile)
					{
						output->tile.tile = input->tile.tile;
					}

					return true;
				}
			} element;

			Manipulator::GetCacheManager()->registerComponent(
				Cache::Sector::Tile,
				&component);
			Manipulator::GetCacheManager()->registerElement(
				Cache::Sector::Tile,
				&element);

			setupTileTemplates();
		}

		~TileMenu()
		{
		}

		bool initialize() override
		{
			return container.initialize();
		}

		void onEvent(
			const sf::Event event)
		{
			container.onEvent(event);
		}

		void onLogic(
			const sf::Time time) { }

		void onDraw()
		{
			MENU::RootBase::onDraw();

			container.onDraw();
		}

		void resetLayout()
		{

		}

	private:
		void setupTileTemplates()
		{
			for (int i = 0; i < (int) Game::Tile::Id::_Length; ++i)
			{
				tiles.push_back(Manipulator::GetTemplate((Game::Tile::Id) i));

				component.addTileButton(
					container.addTileTemplate( tiles.back() )
				);
			}
		}

		TileComponent component;
		TileContainer container;

		std::vector<TileTemplate*> tiles;
	};
}
