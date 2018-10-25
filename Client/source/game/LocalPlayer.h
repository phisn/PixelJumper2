#pragma once

#include <Client/source/game/GameView.h>
#include <Client/source/game/Movement.h>
#include <Client/source/game/PlayerBase.h>

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/InputDevice.h>

namespace Game
{
	class LocalPlayer
		:
		public PlayerBase
	{
	public:
		LocalPlayer(
			const PlayerSettings settings,
			DEVICE::LocalInput* input)
			:
			PlayerBase(settings),
			input(input)
		{
			view.changeTarget(this);
		}

		~LocalPlayer()
		{
		}

		void onLogic(
			const sf::Time time)
		{
			if (input->isSymbolActive(Device::LocalInputSymbol::Trigger))
			{

			}

			if (input->isSymbolActive(Device::LocalInputSymbol::Reset))
			{
				setPosition(sf::Vector2f(6.f, 8.f));

				// ... (need to access world)
			}

			/*
			
				handling other keys elsewhere
			
			*/

			logicMovement(time);
		}

		Movement* changeMovement()
		{
			return &movement;
		}

		const Movement* getMovement() const
		{
			return &movement;
		}

		View* getView()
		{
			return &view;
		}
	private:
		Movement movement;

		void logicMovement(
			const sf::Time time)
		{
			bool x_changed = false;

			if (input->isSymbolActive(Device::LocalInputSymbol::Up) 
				&& movement.isOnGround())
			{
				movement.releaseGround();
				movement.jump(-1.0f);
			}
			
			if (input->isSymbolActive(Device::LocalInputSymbol::Down)
				&& !movement.isOnGround())
			{
				movement.change(time, { 0.f, 0.05f });
			}
			
			if (input->isSymbolActive(Device::LocalInputSymbol::Left))
			{
				movement.change(time, { -0.3f, 0.f });

				x_changed = true;
			}

			if (input->isSymbolActive(Device::LocalInputSymbol::Right))
			{
				movement.change(time, { 0.3f, 0.f });

				x_changed = true;
			}

			movement.onLogic(time, x_changed);
		}

		Game::View view;
		DEVICE::LocalInput* input;
	};
}
