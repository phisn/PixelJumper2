#pragma once

#include <Client/source/device/WindowDevice.h>

#ifndef PJ
#define PJ ::PixelJumper
#endif

namespace PixelJumper
{
	class Player
	{
	public:
		virtual void draw(
			DEVICE::Window* window) = 0;
	};
}
