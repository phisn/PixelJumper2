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

		const Listener& getCore() const
		{
			return coreFunction;
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
				[this](const sf::Time time)
				{ 
					handleTrigger(); 
				}),
			movementRoutine(
				[this](const sf::Time time, Direction direction) 
				{ 
					handleMovement(time, direction); 
				}),
			respawnRoutine(
				[this](const sf::Time time) 
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
			input->onEvent(event);
		}

		void onLogic(const sf::Time time)
		{
			while (input->hasNextCoreKey())
			{
				onCoreSymbol(input->popNextCoreKey(), time);
			}
		}

		InputRoutine<sf::Time> triggerRoutine;
		InputRoutine<sf::Time, Direction> movementRoutine;
		InputRoutine<sf::Time> respawnRoutine;

	private:
		void initializeFromState() override
		{
			PlayerBase::initializeFromState();
			updateView();
		}

		void onCoreSymbol(
			const Device::GameCoreInputSymbol symbol, 
			const sf::Time time)
		{
			switch (symbol)
			{
			case Device::GameCoreInputSymbol::Trigger:
				triggerRoutine.call(time);

				break;
			case Device::GameCoreInputSymbol::Reset:
				respawnRoutine.call(time);

				break;
			case Device::GameCoreInputSymbol::Up:
				movementRoutine.call(time, Direction::Up);

				break;
			case Device::GameCoreInputSymbol::Left:
				movementRoutine.call(time, Direction::Left);

				break;
			case Device::GameCoreInputSymbol::Down:
				movementRoutine.call(time, Direction::Down);

				break;
			case Device::GameCoreInputSymbol::Right:
				movementRoutine.call(time, Direction::Right);

				break;
			}
		}
		
		void onViewSymbol(const Device::GameViewInputSymbol symbol)
		{
			Log::Warning(L"View keys are not implemented yet");
		}

		void handleTrigger()
		{
			Log::Warning(L"Trigger is not implemented yet");
		}

		void handleRespawn()
		{
			Log::Warning(L"Respawn is not implemented yet");
		}

		void handleMovement(
			const sf::Time time,
			const Direction direction)
		{
			switch (direction)
			{
			case Direction::Left:
			case Direction::Right:
				onMovementHorizontal(time, direction);

				break;
			case Direction::Up:
				onMovementJump();

				break;
			case Direction::Down:
				Log::Warning(L"Down is not implemented yet");

				break;
			}
		}

		void onMovementHorizontal(
			const sf::Time time,
			const Direction direction)
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
