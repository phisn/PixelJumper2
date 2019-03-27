#pragma once

#include <Client/source/device/InputDevice.h>

#include <Client/source/game/PlayerBase.h>
#include <Client/source/game/WorldBase.h>

#include <Client/source/logger/Logger.h>

#include <functional>
#include <vector>

namespace Game
{
	enum class InputMode
	{
		Active,  // (pull  based) repeated with time
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

	class LocalPlayer
		:
		public PlayerBase
	{
		LocalPlayer(Device::GameInput* const input)
			:
			PlayerBase(PlayerType::Local),
			input(input)
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

			state.position.addListener(listener);
			state.view_position.addListener(listener);
			state.view_size.addListener(listener);
		}

	public:
		static LocalPlayer* Create(
			Device::GameInput* const input,
			const sf::FloatRect viewPort,
			Resource::PlayerResource* const resource)
		{
			LocalPlayer* localPlayer = new LocalPlayer(input);

			localPlayer->view.setViewport(viewPort);

			return localPlayer;
		}

		void setCurrentWorld(WorldBase* const world)
		{
			currentWorld = world;
			state.worldId = world->getWorldId();
		}

		void onEvent(const sf::Event event)
		{
		}

		void onLogic(const sf::Time time)
		{
			onCoreSymbol(time);
		}

		InputRoutine<sf::Time> triggerRoutine{
			{
				[this](const bool state, const sf::Time time)
				{
					handleTrigger();
				},
				InputMode::Passive
			} };
		InputRoutine<sf::Time, Direction> movementRoutine{
			{
				[this](const bool state, const sf::Time time, Direction direction)
				{
					handleMovement(state, time, direction);
				},
				InputMode::Active
			} };
		InputRoutine<sf::Time> respawnRoutine{ 
			{
				[this](const bool state, const sf::Time time)
				{
					handleRespawn();
				},
				InputMode::Passive
			} };

		void setInputCorrection(const bool mode)
		{
			inputCorrection = mode;
		}

		void setJumpAssist(const float level)
		{
			jumpAssistLevel = level;
		}

		void applyView() const
		{
			Device::Screen::SetView(view);
		}

	private:
		bool inputCorrection = true;
		float jumpAssistLevel = 0.5f;

		void initializeFromState() override
		{
			PlayerBase::initializeFromState();
		}

		// converts sides to correct, real sides dependent
		// on current gravity; { 2; -1 }, (G2) -> (G1)
		CollisionEngine::CollisionInfo::Type correctCollisionTypeByGravity(
			const CollisionEngine::CollisionInfo::Type type)
		{
			const sf::Vector2f gravity = currentWorld->state.readProperties()->gravity;
			const int condition = fabs(gravity.x) > fabs(gravity.y);
			
			return (CollisionEngine::CollisionInfo::Type) ((	
				type + condition + 
					(
						condition ? gravity.x > 0 : gravity.y < 0
					) << 1
				) % 4);
		}

		void onCoreSymbol(
			const sf::Time time)
		{
#define INPUT_STATE_DEF(symbolName, routineName, ...) \
	if (routineName.getCurrentContainer().mode == InputMode::Active) \
	{ \
		routineName.call( \
			input->isKeyPressed(Device::GameCoreInputSymbol::symbolName), \
			time, \
			## __VA_ARGS__); \
	} \
	else \
	{ \
		if (const bool state = input->isKeyPressed(Device::GameCoreInputSymbol::symbolName) \
			; state != routineName.getCurrentState()) \
		{ \
			routineName.call(state, sf::Time(), ## __VA_ARGS__); \
		} \
	}

			INPUT_STATE_DEF(Trigger, triggerRoutine);
			
			INPUT_STATE_DEF(Up, movementRoutine, Direction::Up);
			INPUT_STATE_DEF(Down, movementRoutine, Direction::Down);
			INPUT_STATE_DEF(Left, movementRoutine, Direction::Left);
			INPUT_STATE_DEF(Right, movementRoutine, Direction::Right);

			INPUT_STATE_DEF(Reset, respawnRoutine);
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
			const bool active,
			const sf::Time time,
			const Direction direction)
		{
			switch (direction)
			{
			case Direction::Left:
			case Direction::Right:
				onMovementHorizontal(active, time, direction);

				break;
			case Direction::Up:
				onMovementJump(active);

				break;
			case Direction::Down:
				// Log::Warning(L"Down is not implemented yet");

				break;
			}
		}

		void onMovementHorizontal(
			const bool active,
			const sf::Time time,
			const Direction direction)
		{
			if (active)
			{
				(direction == Direction::Left ? state.hasForceLeft : state.hasForceRight) = true;

				const float movementValue = time.asMicroseconds() / 1000.f
					* 20.f
					* state.readProperties()->speed
					* (1.f / (1.f + state.readProperties()->weight / 1000.f));

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

				state.movement = state.readProperties()->movement + movement * movementValue
					* (direction == Direction::Right ? 1.f : -1.f);
			}
			else
			{
				(direction == Direction::Left ? state.hasForceLeft : state.hasForceRight) = false;
			}
		}

		void onMovementJump(const bool bstate)
		{
			if (bstate)
			{
				const sf::Vector2f tileForce = getTileJumpForce();

				if (tileForce.x == 0 && tileForce.y == 0)
				{
					return;
				}

				state.movement = state.readProperties()->movement
					+ (jumpAssistLevel
						? adjustForceJumpAssist(tileForce)
						: tileForce
						) / state.readProperties()->weight;
			}
		}

		sf::Vector2f adjustForceJumpAssist(const sf::Vector2f tileForce) const
		{	// jumping against gravity, negative gravity
			const sf::Vector2f counterGravityForce = -currentWorld->state.readProperties()->gravity * (float) jumpAssistLevel;

			/*if (counterGravityForce.x > 0 ^ tileForce.x > 0 ||
				counterGravityForce.y > 0 ^ tileForce.y > 0)
			{
				return tileForce;
			}*/

			const float tileForceSum = fabs(tileForce.x) + fabs(tileForce.y);
			const float completeForceSum = tileForceSum + fabs(counterGravityForce.x) + fabs(counterGravityForce.y);
			const float movementForce = fabsf(state.readProperties()->movement.x) + fabsf(state.readProperties()->movement.y);

			const sf::Vector2f gravityDist = counterGravityForce / (completeForceSum + movementForce);
			const sf::Vector2f tileDist = tileForce / completeForceSum;

			return gravityDist * tileForceSum + tileDist * tileForceSum;
		}

		sf::Vector2f getTileJumpForce() const
		{
			sf::Vector2f result = { };

			if (collisionContainer.has(CollisionEngine::CollisionInfo::G3))
			{
				result.x = collisionContainer[CollisionEngine::CollisionInfo::G3]->getDensity();
			}

			if (collisionContainer.has(CollisionEngine::CollisionInfo::G4))
			{
				result.y = collisionContainer[CollisionEngine::CollisionInfo::G4]->getDensity();
			}

			if (collisionContainer.has(CollisionEngine::CollisionInfo::G1))
			{
				result.x -= collisionContainer[CollisionEngine::CollisionInfo::G1]->getDensity();
			}

			if (collisionContainer.has(CollisionEngine::CollisionInfo::G2))
			{
				result.y -= collisionContainer[CollisionEngine::CollisionInfo::G2]->getDensity();
			}

			return result;
		}

		Device::GameInput* const input;

		sf::View view;
		WorldBase* currentWorld;

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
			view.setCenter(state.readProperties()->position);
			view.setSize({ 50, 50 });
		}
	};
}
