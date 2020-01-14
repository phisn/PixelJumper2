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
		/*
			Return: repeated movement without physics,
			used for sliding or bouncing (normal walls slide too [ignores phyiscs])
		*/
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

	protected:
		void registerCollisionType(
			Environment* const env,
			const CollisionType type) // alternative to register
		{
			env->registerTile<CollidableTile>(this);
			env->registerCollisionType(type, this);
		}

		void notifyCollisionEvent(
			const CollisionType* type,
			const Collision* collision)
		{
			collisionEvent.notify(type, collision);
		}
	};
}
