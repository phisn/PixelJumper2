#pragma once

#include <Client/source/menu/RootBase.h>

#include <Client/source/editor/tilemenu/TileComponent.h>
#include <Client/source/editor/tilemenu/TileContainer.h>

#include <Client/source/editor/template/WallTemplate.h>

namespace Editor
{
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

			Manipulator::getCacheManager()->registerComponent(
				Cache::Sector::Tile,
				&component);

			setupTileTemplates();
		}

		~TileMenu()
		{
			for (TileTemplate* const tile : tiles)
			{
				delete tile;
			}
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
			addTileTemplate<WallTemplate>();
		}

		template <class Template>
		void addTileTemplate()
		{
			tiles.push_back( new Template() );

			component.addTileButton(
				container.addTileTemplate( tiles.back() )
			);
		}

		TileComponent component;
		TileContainer container;

		std::vector<TileTemplate*> tiles;
	};
}
