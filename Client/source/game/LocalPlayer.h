#pragma once

#include <Client/source/device/InputDevice.h>

#include <Client/source/game/PlayerBase.h>
#include <Client/source/game/WorldBase.h>

#include <Client/source/logger/Logger.h>

#include <functional>
#include <vector>

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
		std::vector<Listener> listeners;
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
			input->onEvent(event);
		}

		void onLogic(const sf::Time time)
		{
			while (input->hasNextCoreKey())
			{
				onCoreSymbol(input->popNextCoreKey(), time);
			}
		}

		InputRoutine<sf::Time> triggerRoutine{
				[this](const sf::Time time)
				{
					handleTrigger();
				} };
		InputRoutine<sf::Time, Direction> movementRoutine{
				[this](const sf::Time time, Direction direction)
				{
					handleMovement(time, direction);
				} };
		InputRoutine<sf::Time> respawnRoutine{ 
				[this](const sf::Time time)
				{
					handleRespawn();
				} };

		void setInputCorrection(const bool mode)
		{
			inputCorrection = mode;
		}

		void setJumpAssist(const bool mode)
		{

		}

	private:
		bool inputCorrection = true;
		bool jumpAssist = true;

		void initializeFromState() override
		{
			PlayerBase::initializeFromState();
			updateView();
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
			const float movementValue = time.asMicroseconds() / 1000 
				* (1 / state.readProperties()->weight);

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

			state.movement = state.readProperties()->movement + movement
				* (direction == Direction::Right ? 1.f : -1.f);
		}

		void onMovementJump()
		{
			const sf::Vector2f tileForce = getTileJumpForce();

			if (tileForce.x == 0 && tileForce.y == 0)
			{
				return;
			}

			state.movement = state.readProperties()->movement
				+ (jumpAssist
					? adjustForceJumpAssist(tileForce)
					: tileForce
				) / state.readProperties()->weight;
		}

		sf::Vector2f adjustForceJumpAssist(const sf::Vector2f tileForce) const
		{	// jumping against gravity, negative gravity
			const sf::Vector2f counterGravityForce = -currentWorld->state.readProperties()->gravity;

			const float tileForceSum = fabs(tileForce.x) + fabs(tileForce.y);
			const float completeForceSum = tileForceSum + fabs(counterGravityForce.x) + fabs(counterGravityForce.y);

			const sf::Vector2f gravityDist = counterGravityForce / completeForceSum;
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

		sf::View view;
		Device::GameInput* const input;
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

		}
	};
}
