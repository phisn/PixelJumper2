#pragma once

/*
#include <Client/source/menu/MenuBase.h>

#include <Client/source/device/ScreenDevice.h>

#ifndef MENU
#define MENU ::Menu
#endif

namespace Menu
{
	class MenuRootBase
	{
	public:
		virtual bool initialize() = 0;

		virtual void onEvent(
			const sf::Event event) = 0;
		virtual void onLogic(
			const sf::Time time) = 0;
		virtual void onDraw()
		{
			Device::Screen::SetView(view);
		}

		void resetSize()
		{
			view.setSize(
				Device::Screen::GetWindow()->getSize().x * view.getViewport().width,
				Device::Screen::GetWindow()->getSize().y * view.getViewport().height
			);

			view.setCenter(
				view.getSize() / 2.f
			);
		}

		void setViewport(
			const sf::FloatRect viewport)
		{
			view.setViewport(viewport);

			resetSize();
			resetLayout();
		}

	    sf::View* getView()
		{
			return &view;
		}

		// size & position
		virtual void resetLayout() = 0;
	protected:
		bool useOnEvent = false;
		bool useOnLogic = false;

	private:
		sf::View view;
	};
}
*/
