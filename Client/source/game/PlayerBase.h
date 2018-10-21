#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <SFML/Graphics.hpp>

#ifndef GAME
#define GAME ::Game
#endif

namespace Game
{
	struct PlayerSettings
	{
		sf::Color color;
		std::wstring name;
	};

	class PlayerBase
	{
	public:
		PlayerBase(
			const PlayerSettings properties)
			:
			properties(properties)
		{
			shape.setSize(
				sf::Vector2f(1.f, 1.f));
			shape.setFillColor(
				properties.color);
		}

		void onDraw() const
		{
			DEVICE::Interface::getScreen()->onDraw(&shape);
		}

		void setPosition(
			const sf::Vector2f position)
		{
			shape.setPosition(position);
		}

		const sf::Vector2f getPosition() const
		{
			return shape.getPosition();
		}
	private:
		const PlayerSettings properties;

		sf::RectangleShape shape;
	};
}
