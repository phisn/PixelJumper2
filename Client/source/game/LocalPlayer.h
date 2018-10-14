#pragma once

#include <Client/source/game/GameView.h>
#include <Client/source/game/PlayerBase.h>

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/InputDevice.h>

#define IF_HAS_ACTION(source, except) if ((int) source & (int) except)

namespace Game
{
	struct PlayerProperties
	{
		sf::Vector2f movement;
		bool isOnGround;
	};

	class LocalPlayer
		:
		public PlayerBase
	{
	public:
		LocalPlayer(
			DEVICE::LocalInput* input)
			:
			input(input)
		{
		}

		~LocalPlayer();

		void onLogic(
			const sf::Time time)
		{
			if (input->isSymbolActive(Device::LocalInputSymbol::Trigger))
			{

			}

			if (input->isSymbolActive(Device::LocalInputSymbol::Reset))
			{
				setPosition(sf::Vector2f(0.f, 0.f));

				// ... (need to access world)
			}

			/*
			
				handling other keys elsewhere
			
			*/

			logicMovement(time);
		}

		PlayerProperties* changeProperties()
		{
			return &properties;
		}

		const PlayerProperties* getProperties() const
		{
			return &properties;
		}

		View* getView()
		{
			return &view;
		}
	private:
		PlayerProperties properties;

		void logicMovement(
			const sf::Time time)
		{
			if (input->isSymbolActive(Device::LocalInputSymbol::Up) 
				&& properties.isOnGround)
			{
				properties.movement.y = 1.f;
			}
			
			if (input->isSymbolActive(Device::LocalInputSymbol::Down)
				&& !properties.isOnGround)
			{ // Maybe used for platfroms (like terraria)
				properties.movement.y -= 0.05;
			}
			
			if (input->isSymbolActive(Device::LocalInputSymbol::Left))
			{
				properties.movement.x -= properties.movement.x > 0.f 
					? 0.15f : 0.1f;
			}

			if (input->isSymbolActive(Device::LocalInputSymbol::Right))
			{
				properties.movement.x += properties.movement.x < 0.f 
					? 0.15f : 0.1f;
			}
		}

		Game::View view;
		DEVICE::LocalInput* input;
	};
}
