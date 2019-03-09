#pragma once

#include <Client/source/device/InputDevice.h>

#include <Client/source/game/PlayerBase.h>

#include <Client/source/logger/Logger.h>

#include <functional>
#include <unordered_set>

namespace Game
{
	template <typename... Args>
	class InputRoutine
	{
		friend class LocalPlayer;
		typedef std::function<void(Args...)> Listener;
	public:
		InputRoutine(const Listener core)
			:
			coreFunction(core)
		{
		}

		void addListener(const Listener listener)
		{
			listeners.insert(listener);
		}

		void removeListener(const Listener listener)
		{
			listeners.erase(listener);
		}

		void hook(
			const Listener function,
			const bool callListeners = true)
		{
			coreFunction = function;
			this->callListeners = callListeners;
		}

		void unhook()
		{
			coreFunction = defaultFunction;
			callListeners = true;
		}

	private:
		void call(Args... args)
		{
			if (callListeners)
				for (const Listener& listener : listeners)
				{
					listener(args...);
				}

			defaultFunction(args...);
		}

		Listener coreFunction;
		const Listener defaultFunction;

		bool callListeners;
		std::unordered_set<Listener> listeners;
	};

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
			PlayerBase(PlayerType::Local),
			input(input),
			triggerRoutine(
				[this]() 
				{ 
					handleTrigger(); 
				}),
			movementRoutine(
				[this](const Direction direction) 
				{ 
					handleMovement(direction); 
				}),
			respawnRoutine(
				[this]() 
				{ 
					handleRespawn(); 
				})
		{
			state.view_rotation.addListener([this](const float)
				{
					view.setRotation(state.readProperties()->View.rotation);
				});

			state.view_useCustomPosition.addListener([this](const bool)
				{
					updateView();
				});

			const std::function<void(const sf::Vector2f)> listener = [this](const sf::Vector2f)
			{
				updateView();
			};

			state.view_position.addListener(listener);
			state.view_size.addListener(listener);
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

		InputRoutine<> triggerRoutine;
		InputRoutine<Direction> movementRoutine;
		InputRoutine<> respawnRoutine;

	private:
		void initializeFromState() override
		{
			PlayerBase::initializeFromState();
			updateView();
		}

		void onCoreSymbol(const Device::GameCoreInputSymbol symbol)
		{
			switch (symbol)
			{
			case Device::GameCoreInputSymbol::Trigger:
				triggerRoutine.call();

				break;
			case Device::GameCoreInputSymbol::Reset:
				respawnRoutine.call();

				break;
			case Device::GameCoreInputSymbol::Up:
				movementRoutine.call(Direction::Up);

				break;
			case Device::GameCoreInputSymbol::Left:
				movementRoutine.call(Direction::Left);

				break;
			case Device::GameCoreInputSymbol::Down:
				movementRoutine.call(Direction::Down);

				break;
			case Device::GameCoreInputSymbol::Right:
				movementRoutine.call(Direction::Right);

				break;
			}
		}
		
		void onViewSymbol(const Device::GameViewInputSymbol symbol)
		{
			Log::Warning(L"Trigger is not implemented yet");
		}

		void handleTrigger()
		{
		}

		void handleRespawn()
		{
		}

		void handleMovement(const Direction direction)
		{
			switch (direction)
			{
			case Direction::Left:
			case Direction::Right:
				onMovementHorizontal(direction);

				break;
			case Direction::Up:
				onMovementJump();

				break;
			case Direction::Down:
				Log::Warning(L"Down is not implemented yet");

				break;
			}
		}

		void onMovementHorizontal(const Direction direction)
		{
			Log::Warning(L"Movement Horizontal is not implemented yet");
		}

		void onMovementJump()
		{
			Log::Warning(L"Jump is not implemented yet");
		}

		sf::View view;
		Device::GameInput* const input;

		void updateView()
		{
			if (state.readProperties()->View.useCustomPosition)
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
