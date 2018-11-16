#pragma once

#include <Client/source/editor/tilemenu/TileMenuButton.h>

#include <Client/source/menu/container/ScrollContainer.h>
#include <Client/source/menu/style/DefaultStyles.h>

#include <SFML/Graphics/View.hpp>

namespace Editor
{
	class TileContainer
	{
		typedef MENU::ScrollContainer<
			MENU::DefaultStyle::ScrollBar<MENU::ScrollBarBase>,
			MENU::DefaultStyle::RowContainer<MENU::RowContainerBase>
		> Container;
	public:
		TileContainer(
			sf::View* const view)
			:
			view(view),
			container(MENU::Direction::Vertical)
		{
			initializeContainer(view);
		}
		
		~TileContainer()
		{
			for (MENU::ElementBase* const element : container)
			{
				delete element;
			}
		}

		void onEvent(
			const sf::Event event)
		{
			container.onEvent(event);
		}

		void onDraw()
		{
			container.onDraw();
		}

		TileMenuButton* addTileTemplate(
			TileTemplate* const tile)
		{
			TileMenuButton::Style style;

			style.clickColor = sf::Color::Color(255, 0, 0);
			style.fillColor	 = sf::Color::Color(0, 255, 0);
			style.tile = tile;

			TileMenuButton::Properties properties;

			properties.position		= { 0.0f, 0.0f  };
			properties.size			= { 8.0f, 0.05f };
			properties.view			= view;

			TileMenuButton* button = new TileMenuButton(style);

			container.addElement(button);

			return button;
		}

	private:
		sf::View* const view;

		void initializeContainer(
			sf::View* const view)
		{
			MENU::RowContainerBase::Properties rowContainer;

			rowContainer.position	= { 0.2f, 0.0f };
			rowContainer.size		= { 0.8f, 1.0f };
			rowContainer.view		= view;

			MENU::ScrollBarBase::Properties scrollBar;

			scrollBar.padding	= 2.f; // pixel
			scrollBar.position	= { 0.0f, 0.0f };
			scrollBar.size		= { 0.2f, 1.f };
			scrollBar.view		= view;

			Container::Properties properties;

			properties.position		= { 0.0f, 0.0f };
			properties.size			= { 1.0f, 1.0f };
			properties.view			= view;

			properties.rowContainer = &rowContainer;
			properties.scrollBar = &scrollBar;

			container.initialize(&properties);
		}

		Container container;
	};
}
