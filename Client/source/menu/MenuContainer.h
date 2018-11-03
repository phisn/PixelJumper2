#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/menu/MenuChildContainer.h>

namespace Menu
{
	class Container
		:
		public ChildContainer
	{
	public:
		virtual ~Container() = 0;

		virtual void onDraw() override
		{
			DEVICE::Interface::getScreen()->applyView(
				&view);

			ChildContainer::onDraw();
		}

	private:
		sf::View view;
	};
}
