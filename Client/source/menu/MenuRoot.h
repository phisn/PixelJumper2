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

		sf::View* getView()
		{
			return &view;
		}

	protected:
		sf::View view;
	};
}
