#pragma once

#include <Client/source/device/ScreenDevice.h>

#ifndef PJ
#define PJ ::PixelJumper
#endif

namespace PixelJumper
{
	class Player
	{
	public:
		Player(
			sf::Color color)
		{
			shape.setFillColor(color);
			shape.setSize({ 100.f, 100.f });
		}
		
		void draw()
		{
			DEVICE::Screen::getWindow()->draw(shape);
		}

		sf::Vector2f getPosition() const
		{
			return shape.getPosition();
		}

		virtual void onUpdate(
			sf::Time time) = 0;
	protected:
		sf::RectangleShape shape;
	};
}
