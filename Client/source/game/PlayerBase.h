#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <SFML/Graphics.hpp>

namespace Game
{
	class PlayerBase
	{
	public:
		PlayerBase();
		~PlayerBase();

		void draw() const
		{
			DEVICE::Interface::getScreen()->draw(&shape);
		}

		void move(
			const sf::Vector2f offset)
		{
			shape.move(offset);
		}

		void setPosition(
			const sf::Vector2f position)
		{
			shape.setPosition(position);
		}

		const sf::Vector2f getPosition() const
		{
			shape.getPosition();
		}
	private:
		sf::RectangleShape shape;
	};
}
