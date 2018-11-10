#pragma once

#include <Client/source/menu/MenuBase.h>

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#ifndef MENU
#define MENU ::Menu
#endif

namespace Menu
{
	class RootBase
	{
	public:
		virtual void onEvent(
			const sf::Event event) = 0;
		virtual void onLogic(
			const sf::Time time) = 0;
		virtual void onDraw()
		{
			DEVICE::Interface::getScreen()->applyView(&view);
		}

		void setViewport(
			const sf::FloatRect viewport)
		{
			view.setViewport(viewport);

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
