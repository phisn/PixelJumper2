#pragma once

#define USE_POLICIES

#include <Client/source/editor/tilemenu/TileMenuButton.h>

#include <Client/source/menu/container/VerticalScrollContainer.h>
#include <Client/source/menu/RootBase.h>
#include <Client/source/menu/style/DefaultStyles.h>

#include <vector>

namespace Editor
{
	class TileMenu
		:
		public MENU::RootBase
	{
	public:
		typedef MENU::VerticalScrollContainer<
			MENU::DefaultStyle::ScrollBar,
			MENU::DefaultStyle::RowContainer> Container;

		TileMenu()
			:
			container(
				NULL,
				getView()
			)
		{
		}

		void onEvent(
			const sf::Event event) override;
		void onLogic(
			const sf::Time time) override;

		void resetLayout() override
		{
			// all values as pixel
			// setup values
			const float scrollBarSize = 30.f;
			const float scrollBarWidth = 2.f;
			const float containerDistance = 0.f;

			// setup properties
			static Container::Properties properties;

			properties.position = { 0.f, 0.f };
			properties.size = getView()->getSize();

			properties.scrollBar.padding = 2.f;
			properties.scrollBar.position = { 0.f, 0.f };
			properties.scrollBar.size =
			{
				scrollBarSize,
				properties.size.y
			};

			properties.container.position = 
			{ 
				scrollBarSize + containerDistance,
				0.f 
			};
			properties.container.size =
			{
				properties.size.x - scrollBarSize,
				properties.size.y
			};

			// setup container elements
			for (TileMenuButton& element : elements)
			{
				// ...
			}
		}

	private:
		void createElements()
		{
			// ...
			// add from templates
		}

		std::vector<
			TileMenuButton> elements;
		Container container;
	};
}
