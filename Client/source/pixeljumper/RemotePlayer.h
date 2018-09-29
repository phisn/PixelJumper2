#pragma once

#include "Player.h"

#ifndef PJ
#define PJ ::PixelJumper
#endif

namespace PixelJumper
{
	class RemotePlayer
		:
		public Player
	{
	public:
		void onUpdate(
			sf::Time time) override
		{

		}
	};
}
