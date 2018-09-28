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
		Player(sf::Color color)
		{
			shape.setFillColor(color);
		}
		
		void draw()
		{
		}
	private:
		sf::RectangleShape shape;
	};
}
