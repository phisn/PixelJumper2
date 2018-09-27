#pragma once

#include "Player.h"

#ifndef PJ
#define PJ ::PixelJumper
#endif

namespace PixelJumper
{
	enum class Movement
	{
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	class LocalPlayer
		:
		public Player
	{
	public:
		LocalPlayer();
		~LocalPlayer();

		virtual void onUpdate(
			sf::Time time) = 0;
		virtual void onMovement(
			Movement movement) = 0;
	};
}
