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

		sf::Vector2f getRemainOffset() const
		{
			return sf::Vector2f(
				target.x - info.position.x,
				target.y - info.position.y
			);
		}
	};

	class CollidableTile
	{
	public:
		CollidableTile(
			const float density,
			const float inputForceAddition,
			const float friction)
			:
			density(density),
			inputForceAddition(inputForceAddition),
			friction(friction)
		{
		}

		/*
			Return: repeated movement without physics,
			used for sliding or bouncing (normal walls slide too [ignores phyiscs])
		*/

		void setDensity(float value)
		{
			density = value * Definition::tile_density;
		}
		void setFriction(float value)
		{
			friction = value * Definition::tile_friction;
		}
		void setForce(float value)
		{
			inputForceAddition = value * Definition::tile_input;
		}

		virtual sf::Vector2f onCollision(
			const CollisionType type,
			const Collision collision) = 0;
		
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
		float friction;

		void notifyCollisionEvent(
			const CollisionType* type,
			const Collision* collision)
		{
			collisionEvent.notify(type, collision);
		}
	};
}
