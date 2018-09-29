#pragma once

#include "Player.h"

#include <Client/source/device/InputDevice.h>

#ifndef PJ
#define PJ ::PixelJumper
#endif

namespace PixelJumper
{
	class LocalPlayer
		:
		public Player
	{
	public:
		LocalPlayer(const Device::Input input)
			:
			input(input)
		{
		}

		void onUpdate(
			sf::Time time) override
		{

		}
	private:
		Device::Input input;
	};
}
