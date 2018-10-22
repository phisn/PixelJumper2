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
				setPosition(sf::Vector2f(0.f, 0.f));

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
			bool slowdown_x = true, slowdown_y = true;

			if (input->isSymbolActive(Device::LocalInputSymbol::Up) 
				&& movement.isOnGround())
			{
				movement.releaseGround();
				movement.jump(-2.f);

				slowdown_y = false;
			}
			
			if (input->isSymbolActive(Device::LocalInputSymbol::Down)
				&& !movement.isOnGround())
			{
				movement.change(time, { 0.f, 10.f });

				if (movement.getMovement().y > 0.f)
				{
					slowdown_y = false;
				}
			}
			
			if (input->isSymbolActive(Device::LocalInputSymbol::Left))
			{
				movement.change(time, { -0.6f, 0.f });

				if (movement.getMovement().x < 0.f)
				{
					slowdown_x = false;
				}
			}

			if (input->isSymbolActive(Device::LocalInputSymbol::Right))
			{
				movement.change(time, { 0.6f, 0.f });

				if (movement.getMovement().x > 0.f)
				{
					slowdown_x = false;
				}
			}

			movement.time(time, slowdown_x, slowdown_y);
		}

		Game::View view;
		DEVICE::LocalInput* input;
	};
}
