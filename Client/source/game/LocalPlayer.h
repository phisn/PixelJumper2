#pragma once

#include <Client/source/device/InputDevice.h>
#include <Client/source/game/PlayerBase.h>

namespace Game
{
	class LocalPlayer
		:
		public Player
	{
	public:
		void onLogic(const sf::Time time) override
		{
		}

		void onEvent(const sf::Event event)
		{
			if (event.type == sf::Event::KeyPressed)
			{
				if (Device::GameCoreInputSymbol symbol = input->convertToCoreSymbol(event.key.code)
					; symbol != Device::GameCoreInputSymbol::_Invalid)
				{
					onCoreSymbol(symbol);

					return;
				}

				if (Device::GameViewInputSymbol symbol = input->convertToViewSymbol(event.key.code)
					; symbol != Device::GameViewInputSymbol::_Invalid)
				{
					onViewSymbol(symbol);

					return;
				}

			}
		}

	private:
		void onCoreSymbol(const Device::GameCoreInputSymbol symbol)
		{
			switch (symbol)
			{
			case Device::GameCoreInputSymbol::Trigger:
				_N_Simulator::Player::GetTriggerRoutineContainer()
					->CallRoutine(&state);

				break;
			case Device::GameCoreInputSymbol::Reset:
				Log::Warning(L"Reset button is not implemented yet");

				break;
			case Device::GameCoreInputSymbol::Up:
				_N_Simulator::Player::GetMovementRoutineContainer()
					->CallRoutine(&state, Game::Direction::Up);

				break;
			case Device::GameCoreInputSymbol::Left:
				_N_Simulator::Player::GetMovementRoutineContainer()
					->CallRoutine(&state, Game::Direction::Left);

				break;
			case Device::GameCoreInputSymbol::Down:
				_N_Simulator::Player::GetMovementRoutineContainer()
					->CallRoutine(&state, Game::Direction::Down);

				break;
			case Device::GameCoreInputSymbol::Right:
				_N_Simulator::Player::GetMovementRoutineContainer()
					->CallRoutine(&state, Game::Direction::Right);

				break;
			}
		}
		
		void onViewSymbol(const Device::GameViewInputSymbol symbol)
		{
		}

		sf::View view;
		Device::GameInput* const input;
	};
}
