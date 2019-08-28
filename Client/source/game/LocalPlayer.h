#pragma once

#include <Client/source/device/InputDevice.h>

#include <Client/source/game/GamePlayer.h>

#include <Client/source/game/tiletrait/CollidableTile.h>

namespace Game
{
	enum class InputMode
	{
		Active,  // (pull  based) repeated with time
		Hybrid,  // (pull  based) repeated with time if state is true
		Passive  // (event based) not repeated without time
	};

	template <typename... Args>
	class InputRoutine
	{
		friend class LocalPlayer;
		typedef std::function<void(const bool, Args...)> Listener;

	public:
		struct FunctionContainer
		{
			Listener function;

			InputMode mode = Passive;
			bool callListeners = true;
		};

		InputRoutine(const FunctionContainer defaultFunction)
			:
			defaultFunction(defaultFunction),
			currentFunction(defaultFunction)
		{
		}

		void addListener(const Listener listener)
		{
			listeners.push_back(listener);
		}

		void removeListener(const Listener listener)
		{
			for (decltype(listeners)::iterator iterator = listeners.begin()
				; iterator != listeners.end(); ++iterator)
				if (iterator->target<void>() == listener.target<void>())
				{
					listeners.erase(iterator);

					break;
				}
		}

		void hook(FunctionContainer function)
		{
			currentFunction = function;
		}

		void unhook()
		{
			currentFunction = defaultFunction;
		}

		const FunctionContainer& getCurrentContainer() const
		{
			return currentFunction;
		}

		const FunctionContainer& getDefaultContainer() const
		{
			return defaultFunction;
		}

		bool getCurrentState() const
		{
			return currentState;
		}

	private:
		void update(const bool state, Args... args)
		{
			switch (currentFunction.mode)
			{
			case InputMode::Active:
				call(state, args...);

				break;
			case InputMode::Hybrid:
				if (state)
				{
					call(state, args...);
				}

				break;
			case InputMode::Passive:
				if (state != currentState)
				{
					call(state, args...);
				}
			}
		}

		void call(const bool state, Args... args)
		{
			currentState = state;

			if (currentFunction.callListeners)
				for (const Listener& listener : listeners)
				{
					listener(state, args...);
				}

			currentFunction.function(state, args...);
		}

		std::vector<Listener> listeners;

		const FunctionContainer defaultFunction;
		FunctionContainer currentFunction;

		bool currentState;
	};

	enum class InputDirection
	{
		Up,
		Left,
		Down,
		Right
	};
	
	class LocalPlayer
		:
		public PlayerBase
	{
	public:
		LocalPlayer(
			const Device::Input::PlayerId playerId,
			const Resource::PlayerResource* const resource)
			:
			PlayerBase(resource),
			input(
				Device::Input::GetGameInput(playerId)
			)
		{
		}

		void onLogic(const sf::Time time) override
		{
			handleInput(time);

			// maybe something wants
			// to react on handleInput
			// changes
			PlayerBase::onLogic(time);
		}

		// InputRoutine<sf::Time> triggerRoutine;
		InputRoutine<sf::Time, InputDirection> movementRoutine{
			{
				[this](const bool state, const sf::Time time, const InputDirection input)
				{
					handleMovement(state, time, input);
				},
				InputMode::Active,
				true
			} };

		Property<bool> inputCorrection;
		Property<bool> jumpAssistLevel;

	private:
		void handleInput(const sf::Time time)
		{
			movementRoutine.update(input->isKeyPressed(Device::GameCoreInputSymbol::Up), time, InputDirection::Up);
			movementRoutine.update(input->isKeyPressed(Device::GameCoreInputSymbol::Left), time, InputDirection::Left);
			movementRoutine.update(input->isKeyPressed(Device::GameCoreInputSymbol::Down), time, InputDirection::Down);
			movementRoutine.update(input->isKeyPressed(Device::GameCoreInputSymbol::Right), time, InputDirection::Right);
		}

		void handleMovement(
			const bool active,
			const sf::Time time,
			const InputDirection direction)
		{
			switch (direction)
			{
			case InputDirection::Left:
			case InputDirection::Right:
				onMovementHorizontal(active, time, direction);

				break;
			case InputDirection::Up:
				onMovementJump(active);

				break;
			case InputDirection::Down:
				// Log::Warning(L"Down is not implemented yet");

				break;
			}
		}

		void onMovementHorizontal(
			const bool active,
			const sf::Time time,
			const InputDirection direction)
		{
			if (properties.inputReducedFriction != active)
			{
				properties.inputReducedFriction.setValue(active);
			}

			if (active)
			{
				const float movementValue = time.asMicroseconds() / 1000.f
					* 10.f
					* properties.speed
					* (1.f / (1.f + properties.weight / 1000.f));

				/*

					Movement distribution dependent on gravity

					Dx
				   +----+
				   | D /
				Dy |  /+-------->       [Dy] scales with [gx]
				   | / |       gx       [Dx] scales with [gy]
				   |/  |
				   +   v gy



							 D  * gx
					  Dy = ----------- = (D * gx) / (gx + gy)
						   |gx| + |gy|

							 D  * gy
					  Dx = ----------- = (D * gy) / (gx + gy)
						   |gx| + |gy|

				*/
				const sf::Vector2f gravity = currentWorld->state.readProperties()->gravity;
				const float gravitySum = fabsf(gravity.x) + fabsf(gravity.y);

				sf::Vector2f movement =
				{
					(movementValue * gravity.y) / gravitySum,
					(movementValue * gravity.x) / gravitySum
				};

				if (inputCorrection && gravity.y < 0)
				{
					movement.x = -movement.x;
				}

				properties.movement = *properties.movement + movement
					* (direction == InputDirection::Right ? 1.f : -1.f);
			}
		}

		void onMovementJump(const bool active)
		{
			if (active)
			{
				const sf::Vector2f tileForce = getTileJumpForce();

				if (tileForce.x == 0 && tileForce.y == 0)
				{
					return;
				}

				properties.movement += (jumpAssistLevel
					? adjustForceJumpAssist(tileForce)
					: tileForce
				) / *properties.weight;
			}
		}

		sf::Vector2f adjustForceJumpAssist(const sf::Vector2f tileForce) const
		{

		}

		sf::Vector2f getTileJumpForce() const
		{
			return
			{
				getSingleJumpForce(CollisionEngine::CollisionInfo::G3) -
				getSingleJumpForce(CollisionEngine::CollisionInfo::G1),

				getSingleJumpForce(CollisionEngine::CollisionInfo::G4) -
				getSingleJumpForce(CollisionEngine::CollisionInfo::G2),
			};
		}

		float getSingleJumpForce(const CollisionEngine::CollisionInfo::Type type) const
		{
			return collisionContainer.has(type)
				? collisionContainer[type]->getDensity()
				: 0;
		}

		Device::GameInput* const input;
	};
}
