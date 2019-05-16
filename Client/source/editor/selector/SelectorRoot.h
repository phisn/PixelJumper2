#pragma once

#include <Client/source/device/ScreenDevice.h>

#include <Client/source/menu/MenuRootBase.h>
#include <Client/source/menu/shape/CrossShape.h>

namespace Editor
{
	class SelectorView
	{
	public:
		SelectorView(sf::View& view)
			:
			view(view)
		{
		}

		void beginMouseMovement(const sf::Vector2i position)
		{
			beginMouseOffset = position;
			beginViewCenterOffset = view.getCenter();
		}

		void nextMouseMovement(const sf::Vector2i nextPosition)
		{
			const sf::Vector2f nextCoordsPosition = 
				Device::Screen::GetWindow()->mapPixelToCoords(nextPosition, view);

			const sf::Vector2f beginCoordsPosition =
				Device::Screen::GetWindow()->mapPixelToCoords(beginMouseOffset, view);

			view.setCenter(
				beginViewCenterOffset + 
				(nextCoordsPosition - beginCoordsPosition)
			);
		}


		
	private:
		sf::Vector2i beginMouseOffset;
		sf::Vector2f beginViewCenterOffset;

		sf::View& view;
	};

	class SelectorRoot
		:
		public Menu::MenuRootBase
	{
	public:
		SelectorRoot()
			:
			MenuRootBase(),
			selectorView(view)
		{
		}

		bool build() override
		{

			return true;
		}

	private:
		SelectorView selectorView;
	};
}
