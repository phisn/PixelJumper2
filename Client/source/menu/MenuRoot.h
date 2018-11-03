#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/menu/MenuBase.h>

namespace Menu
{
	class Root
		:
		public Base
	{
	public:
		virtual ~Root() { };

		virtual void onDraw() override
		{
			DEVICE::Interface::getScreen()->applyView(
				&view);
		}

		virtual void setViewport(
			const sf::FloatRect viewport)
		{
			view.setViewport(viewport);
		}

	protected:
		sf::View view;
	};
}
