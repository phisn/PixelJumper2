#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/game/PlayerState.h>

#include <string>

#include <SFML/Graphics.hpp>

namespace Game
{
	class PlayerBase
	{
	public:
		virtual void onDraw()
		{
			Device::Interface::GetScreen()->onDraw(&Visual.basicShape);
		}

		virtual void onLogic(const sf::Time time)
		{
			const float miliTime = time.asMicroseconds() / 1000.f;

			sf::Vector2f offset = state.Properties.movement * state.Properties.speed * miliTime;

			updateVisual();
		}

	private:
		PlayerState state;

		void updateVisual()
		{
			Visual.basicShape.setPosition(state.Properties.position);
		}

		struct
		{
			sf::RectangleShape basicShape;

		} Visual;

		std::wstring name;
	};
}
