#pragma once

#include <Client/source/device/InputDevice.h>
#include <Client/source/game/PlayerBase.h>

namespace Game
{
	class LocalPlayer
		:
		public PlayerBase
	{
	public:
		void onLogic(const sf::Time time) override
		{
		}

	private:
		Device::LocalInput li;
	};
}
