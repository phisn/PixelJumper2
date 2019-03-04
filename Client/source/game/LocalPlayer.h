#pragma once

#include <Client/source/device/InputDevice.h>
#include <Client/source/game/PlayerBase.h>

namespace Game
{
	/*
	
	
		Routine :c
	
	
	*/

	class LocalPlayer
		:
		public PlayerBase
	{
	public:
		static LocalPlayer* Create(
			Device::GameInput* const input,
			const sf::FloatRect viewPort,	
			const Resource::PlayerResource resource)
		{
			LocalPlayer* localPlayer = new LocalPlayer(input);

			localPlayer->view.setViewport(viewPort);

			return localPlayer;
		}

		LocalPlayer(Device::GameInput* const input)
			:
			PlayerBase(),
			input(input)
		{
			view_rotation.addListener([this](const float)
				{
					this->view.setRotation(state.Properties.View.rotation);
				});

			view_useCustomPosition.addListener([this](const bool)
				{
					updateView();
				});

			const std::function<void(const sf::Vector2f)> listener = [this](const sf::Vector2f)
			{
				updateView();
			};

			view_position.addListener(listener);
			view_size.addListener(listener);
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
				::Player::GetTriggerRoutineContainer()
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

		void updateView()
		{
			if (state.Properties.View.useCustomPosition)
			{
				this->update_custom_view();
			}
			else
			{
				this->update_default_view();
			}
		}

		void update_custom_view()
		{
		}

		void update_default_view()
		{

		}
	};
}
