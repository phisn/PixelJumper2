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
			shape.setSize({ 1.f, 1.f });
		}
		
		void draw()
		{
			DEVICE::Screen::getWindow().draw(shape);
		}

		void adjustView(
			int position,
			int count)
		{
			switch (count)
			{
			case 1:
				view.setViewport(sf::FloatRect(
					0.f, 0.f,
					1.f, 1.f)
				);

				break;
			case 2:
				if (position == 1)
				{
					view.setViewport(sf::FloatRect(
						0.f, 0.f,
						0.5f, 1.f));
				}
				else
				{
					view.setViewport(sf::FloatRect(
						0.5f, 0.f,
						0.5f, 1.f));
				}

				break;
			case 3:
			case 4:
				break;
			}
		}

		void applyView()
		{
			view.setCenter(shape.getPosition());
			DEVICE::Screen::getWindow().setView(view);
		}

		virtual void onUpdate(
			sf::Time time) = 0;
	protected:
		sf::View view;
		sf::RectangleShape shape;
	};
}
