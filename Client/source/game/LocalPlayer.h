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
			const PlayerInformation information,
			Device::View* const view)
			:
			PlayerBase(information),
			input(
				Device::Input::GetGameInput(playerId)
			),
			view(view)
		{
			properties.viewFollow.addListener(
				[this](const bool oldViewFollow, 
					   const bool newViewFollow)
				{
					properties.viewWindow.update(true);
				});
			properties.viewWindow.addListener(
				[this](const sf::FloatRect oldViewWindow, 
					   const sf::FloatRect newViewWindow)
				{
					if (properties.viewFollow)
					{
						this->view->setSize(
						{
							properties.viewWindow->width,
							properties.viewWindow->height
						});
					}
					else
					{
						this->view->setRect(newViewWindow);
					}
				});
			properties.viewRotation.addListener(
				[this](const float oldViewRotation, 
					   const float newViewRotation)
				{
					this->view->setRotation(newViewRotation);
				});
			properties.position.addListener(
				[this](const sf::Vector2f oldPosition,
					   const sf::Vector2f newPosition)
				{
					if (properties.viewFollow)
					{
						this->view->setCenter(newPosition);
					}
				});
		}

		void onInternalUpdate() override
		{
			handleInput();
			PlayerBase::onInternalUpdate();
		}

		// InputRoutine<sf::Time> triggerRoutine;
		InputRoutine<InputDirection> movementRoutine{
			{
				[this](const bool state, const InputDirection input)
				{
					handleMovement(state, input);
				},
				InputMode::Active,
				true
			} };

		Property<bool> inputCorrection;
		Property<bool> jumpAssistLevel;

	private:
		void handleInput()
		{
			movementRoutine.update(input->isKeyPressed(Device::GameCoreInputSymbol::Up), InputDirection::Up);
			movementRoutine.update(input->isKeyPressed(Device::GameCoreInputSymbol::Left), InputDirection::Left);
			movementRoutine.update(input->isKeyPressed(Device::GameCoreInputSymbol::Down), InputDirection::Down);
			movementRoutine.update(input->isKeyPressed(Device::GameCoreInputSymbol::Right), InputDirection::Right);
		}

		void handleMovement(
			const bool active,
			const InputDirection direction)
		{
			switch (direction)
			{
			case InputDirection::Left:
			case InputDirection::Right:
				onMovementHorizontal(active, direction);

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
			const InputDirection direction)
		{
/*			if (active)
			{
				// vel = sqrt((m * velc ^ 2 + n) / m)
				// vel = sqrt(velc ^ 2 + n / m)

				const float inForce = properties.inputForce * getForceAddition();
				const sf::Vector2f vecForce = { (direction == InputDirection::Left ? -1 : 1) * inForce, 0.f  };
				properties.movement = vecForce;
			}*/

			
			/*if (properties.inputReducedFriction != active)
			{
				properties.inputReducedFriction.setValue(active);
			}*/

			if (active)
			{
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

				
				
				const float gravitySum = fabsf(gravity.x) + fabsf(gravity.y);

				sf::Vector2f movement =
				{
					(movementValue * gravity.y) / gravitySum,
					(movementValue * gravity.x) / gravitySum
				};

				if (inputCorrection && gravity.y < 0)
				{
					movement.x = -movement.x;
				}*/

				float movementValue = properties.inputForce * getForceAddition()
					* (direction == InputDirection::Right ? 1.f : -1.f);;


				if (signbit(movementValue) == signbit(properties.movement->x))
				{
					movementValue /= *properties.mass;
				}
				else
				{
					if (fabsf(movementValue) < fabsf(properties.movement->x))
					{
						movementValue *= properties.inputReduce;

						if (fabsf(movementValue) > fabsf(properties.movement->x))
							movementValue = -properties.movement->x;
					}
				}

				properties.movement += sf::Vector2f(movementValue, 0);
			}
		}

		float getForceAddition() const
		{
			return getSingleForceAddition(CollisionEngine::CollisionInfo::G1)
				 * getSingleForceAddition(CollisionEngine::CollisionInfo::G2)
				 * getSingleForceAddition(CollisionEngine::CollisionInfo::G3)
				 * getSingleForceAddition(CollisionEngine::CollisionInfo::G4);
		}

		float getSingleForceAddition(const CollisionEngine::CollisionInfo::Type type) const
		{
			return collisionContainer.has(type)
				? collisionContainer[type]->getInputForceAddition()
				: 1;
		}

		void onMovementJump(const bool active)
		{
			if (active)
			{
				const sf::Vector2f jumpForce = getTileJumpForce();

				if (jumpForce.x == 0 && jumpForce.y == 0)
				{
					return;
				}

				properties.movement += jumpForce / *properties.mass;

				/**
				const sf::Vector2f tileForce = getTileJumpForce();

				if (tileForce.x == 0 && tileForce.y == 0)
				{
					return;
				}

				properties.movement += (jumpAssistLevel
					? adjustForceJumpAssist(tileForce)
					: tileForce
				) / *properties.weight;
				**/
			}
		}

		sf::Vector2f adjustForceJumpAssist(const sf::Vector2f tileForce) const
		{
			return { };
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

		Device::View* const view;
		Device::GameInput* const input;
	};
}
