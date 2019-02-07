#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/game/GameWorld.h>
#include <Client/source/game/PlayerState.h>
#include <Client/source/game/Simulator.h>

#include <string>

#include <SFML/Graphics.hpp>

namespace Game
{
	class PlayerBase
	{
	public:
		PlayerState state;

		virtual void onDraw()
		{
			Device::Interface::GetScreen()->onDraw(&Visual.basicShape);
		}

		virtual void onLogic(const sf::Time time)
		{
			const sf::Vector2f targetPosition = state.Properties.position + getTargetOffset(time);

			if (Simulator::GetWorld()->checkPathForCollision(
					state.Properties.position,
					targetPosition)) // collision position & collision tile
			{
				Simulator::TriggerEvent(TileEventType::Collision, this);
			}
			else
			{
				state.Properties.position = targetPosition;
			}

			updateVisual();
		}

	private:
		sf::Vector2f getTargetOffset(const sf::Time time) const
		{
			return	state.Properties.movement
				*	state.Properties.speed
				*	(time.asMicroseconds() / 1000.f
				);
		}

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
