#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <vector>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Time.hpp>

namespace Framework
{
	class Animation
	{
	public:
		bool onUpdate(
			const sf::Time time)
		{
			passed += time;

			while (passed > speed)
			{
				if (spriteCounter + 1 == sprites.size())
				{
					return false;
				}

				++spriteCounter;
				passed -= speed;
			}

			return true;
		}

		void onDraw() const
		{
			DEVICE::Interface::GetScreen()->onDraw(&sprites[spriteCounter]);
		}

		void reset()
		{
			spriteCounter = 0;
			passed = sf::microseconds(0);
		}

	private:
		sf::Time passed, speed;

		int spriteCounter = 0;
		std::vector<sf::Sprite> sprites;
	};

}