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
			setupContainer(view);
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

			style.enterColor = sf::Color::Color(0, 0, 255);
			style.fillColor	 = tile->color;
			style.tile = tile;

			TileMenuButton::Properties properties;

			properties.position		= { 0.0f, 0.0f  };
			properties.size			= { 0.8f, 0.05f };
			properties.view			= view;

			TileMenuButton* button = new TileMenuButton(style);

			button->setup(&properties);
			container.addElement(button);

			return button;
		}

		bool initialize()
		{
			return container.initialize();
		}

	private:
		sf::View* const view;

		void setupContainer(
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

			container.setup(&properties);
		}

		Container container;
	};
}
