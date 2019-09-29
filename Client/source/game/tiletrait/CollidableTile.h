#pragma once

#include <Client/source/game/CollisionEngine.h>
#include <Client/source/game/Environment.h>
#include <Client/source/game/GameEvent.h>
#include <Client/source/game/GamePlayer.h>

#include <utility>
#include <vector>

namespace Game
{
	struct Collision
	{
		CollisionEngine::CollisionInfo info;

		PlayerBase* player;
		sf::Vector2f target; // expected destination

		// float timeValue;
	};

	class CollidableTile
	{
	public:
		CollidableTile(
			const float density,
			const float friction)
			:
			density(density),
			friction(friction)
		{
		}

		/*
			Return: repeated movement without physics,
			used for sliding or bouncing (normal walls slide too [ignores phyiscs])
		*/



		void setDensity(float value)
		{
			density = value;
		}
		void setCrit(float value)
		{
			criticalForce = value;
		}
		void setFriction(float value)
		{
			friction = value;
		}
		void setForce(float value)
		{
			inputForceAddition = value;
		}

		virtual sf::Vector2f onCollision(
			const CollisionType type,
			const Collision collision) = 0;
			/*
		{
			sf::Vector2f movement = collision.player->getProperties().movement;
//			movement = collision.player->getProperties().friction * friction;

			if (fabsf(movement.x) < criticalForce)
			{
				movement.x = 0.f;
			}

			if (fabsf(movement.y) < criticalForce)
			{
				movement.y = 0.f;
			}

			collision.player->getProperties().movement = movement;

			return { };
		}*/
		
		// copied from / same as in GameTileBase.h
		virtual const sf::Vector2f getPosition() const = 0;
		virtual const sf::Vector2f getSize() const = 0;

		GameEvent<
			CollidableTile, 
			const CollisionType*, 
			const Collision*
		> collisionEvent;

		float getDensity() const
		{
			return density;
		}

		float getInputForceAddition() const
		{
			return inputForceAddition;
		}

	protected:
		void registerCollisionType(
			Environment* const env,
			const CollisionType type) // alternative to register
		{
			env->registerTile<CollidableTile>(this);
			env->registerCollisionType(type, this);
		}

		float density;
		float inputForceAddition;
		float criticalForce;
		float friction;

		void notifyCollisionEvent(
			const CollisionType* type,
			const Collision* collision)
		{
			collisionEvent.notify(type, collision);
		}
	};
}
