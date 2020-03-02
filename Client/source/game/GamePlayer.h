#pragma once

#include <Client/source/game/CollisionEngine.h>
#include <Client/source/game/PlayerProperties.h>
#include <Client/source/game/PlayerInformation.h>

#include <Client/source/logger/Logger.h>

namespace Game
{
	class CollisionContainer
	{
	public:
		struct Collider
		{
			Collider(
				float inputForceAddition,
				float jumpForce,
				int frameLength,
				int functLength)
				:
				inputForceAddition(inputForceAddition),
				jumpForce(jumpForce),
				frameLength(frameLength),
				functLength(functLength)
			{
			}

			Collider(
				float inputForceAddition,
				float jumpForce)
				:
				Collider(inputForceAddition, jumpForce, 1, 1)
			{
			}

			Collider()
				:
				Collider(1.f, 1.f, 1, 1)
			{
			}

			float inputForceAddition;
			float jumpForce;

			// a new collision will normally override
			// the old collider regardless of the length
			// or strength. this can be used, to define
			// new game mechanics

			// time in frames the collision is active
			int frameLength;
			// normally the number of times the collider
			// will be used in functional context.
			// eg. jumping
			int functLength;
		};

		CollisionContainer()
		{
			for (int i = 0; i < 4; ++i)
			{
				colliders[i].frameLength = 0;
				colliders[i].functLength = 0;
			}
		}

		void reduceFrame()
		{
			for (int i = 0; i < 4; ++i)
				if (colliders[i].frameLength > 0)
				{
					--colliders[i].frameLength;
				}
		}

		void reduceFunct()
		{
			for (int i = 0; i < 4; ++i)
				if (colliders[i].frameLength > 0)
				{
					--colliders[i].frameLength;
				}
		}

		void setCollider(
			const CollisionEngine::CollisionInfo::Type type,
			const Collider collider)
		{
			colliders[type] = collider;
		}

		const Collider& readCollider(const CollisionEngine::CollisionInfo::Type type) const
		{
			return colliders[type];
		}

		bool isActive(const CollisionEngine::CollisionInfo::Type type) const
		{
			return colliders[type].frameLength != 0 && colliders[type].functLength != 0;
		}

	private:
		Collider colliders[4];
	};

	template <typename... Args>
	class InputRoutine
	{
	public:
		typedef std::function<void(Args...)> Listener;

		InputRoutine(const Listener defaultFunction)
			:
			defaultFunction(defaultFunction),
			currentFunction(defaultFunction)
		{
		}

		void addListener(const Listener listener)
		{
			listeners.push_back(listener);
		}

		void hook(Listener function)
		{
			currentFunction = function;
		}

		void unhook()
		{
			currentFunction = defaultFunction;
		}

		const Listener& getCurrentContainer() const
		{
			return currentFunction;
		}

		const Listener& getDefaultContainer() const
		{
			return defaultFunction;
		}

		void call(Args... args) const
		{
			for (const Listener& listener : listeners)
			{
				listener(args...);
			}

			currentFunction(args...);
		}

	private:
		std::vector<Listener> listeners;

		const Listener defaultFunction;
		Listener currentFunction;
	};

	class PlayerBase
		:
		public GameState
	{
		friend class World;

	public:	
		PlayerBase(const PlayerInformation information)
			:
			information(information)
		{
		}

		// update without time dependent on
		// logic cyclus in world
		virtual void onInternalUpdate()
		{
			properties.update();
		}

		InputRoutine<>  
		   up		{ [this]() { commonJump(); } 
		}, down		{ [this]() { Log::Information(L"Not implemented yet"); } 
		}, left		{ [this]() { commonMovement(true); }
		}, right	{ [this]() { commonMovement(false); }
		}, interact	{ [this]() { Log::Information(L"Not implemented yet"); }
		}, respawn	{ [this]() { commonRespawn(); } 
		};

		bool writeState(Resource::WritePipe* const writePipe)
		{
			return properties.writeState(writePipe);
		}
		
		bool readState(Resource::ReadPipe* const readPipe)
		{
			return properties.readState(readPipe);
		}

		CollisionContainer& getCollisionContainer()
		{
			return collisionContainer;
		}

		PlayerProperties& getProperties()
		{
			return properties;
		}

		const PlayerInformation& getInformation() const
		{
			return information;
		}

	protected:
		PlayerProperties properties;
		CollisionContainer collisionContainer;

	private:
		void commonJump()
		{
			const sf::Vector2f jumpForce = getTileJumpForce();

			if (jumpForce.x == 0 && jumpForce.y == 0)
			{
				return;
			}

			properties.movement += jumpForce / *properties.mass;
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

		float getSingleJumpForce(const CollisionEngine::CollisionInfo::Type type) const;

		// invert right movement to left if inverted
		void commonMovement(const bool inverted)
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

			*/
			/*
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
			*/
			float movementValue = properties.inputForce
				* getForceAddition()
				/ *properties.mass
				* (inverted ? -1.f : 1.f);


			if (signbit(movementValue) != signbit(properties.movement->x) && 
				fabsf(movementValue) < fabsf(properties.movement->x))
			{
				movementValue *= properties.inputReduce;

				if (fabsf(movementValue) > fabsf(properties.movement->x))
					movementValue = -properties.movement->x;
			}

			properties.movement += sf::Vector2f(movementValue, 0);
		}

		float getForceAddition() const
		{
			return getSingleForceAddition(CollisionEngine::CollisionInfo::G1)
				* getSingleForceAddition(CollisionEngine::CollisionInfo::G2)
				* getSingleForceAddition(CollisionEngine::CollisionInfo::G3)
				* getSingleForceAddition(CollisionEngine::CollisionInfo::G4);
		}

		float getSingleForceAddition(const CollisionEngine::CollisionInfo::Type type) const;

		void commonRespawn()
		{
			properties.position = *properties.spawn;
			properties.movement = { 0.f, 0.f };

			properties.update();
		}

		const PlayerInformation information;
	};
}