#pragma once

#include "Player.h"
#include "PlayerView.h"

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
		LocalPlayer(
			const Device::Input input,
			const sf::Color color)
			:
			input(input),
			Player(color)
		{
			view.changePlayer(this);
		}

		void onUpdate(
			sf::Time time) override
		{
			if (input.isKeyPressed(Device::Control::Left))
			{
				shape.move(sf::Vector2f(-0.0007f * time.asMicroseconds(), 0));
			}
			
			if (input.isKeyPressed(Device::Control::Right))
			{
				shape.move(sf::Vector2f(0.0007f * time.asMicroseconds(), 0));
			}
			
			if (input.isKeyPressed(Device::Control::Up))
			{
				shape.move(sf::Vector2f(0, -0.0007f * time.asMicroseconds()));
			}
			
			if (input.isKeyPressed(Device::Control::Down))
			{
				shape.move(sf::Vector2f(0, 0.0007f * time.asMicroseconds()));
			}
		}

		PlayerView& getView()
		{
			return view;
		}
	private:
		PlayerView view;
		Device::Input input;
	};
}
