#pragma once

#include <Client/source/game/CollisionEngine.h>
#include <Client/source/game/GameEvent.h>
#include <Client/source/game/GamePlayer.h>
#include <Client/source/game/tiletrait/TileContent.h>

#include <utility>
#include <vector>

namespace Game
{
	struct Collision
	{
		// expected destination. collision and collision
		// type can be found in collisioninfo
		sf::Vector2f target;

		PlayerBase* player;
		CollisionEngine::CollisionInfo info;

		sf::Vector2f getRemainOffset() const
		{
			return sf::Vector2f(
				target.x - info.position.x,
				target.y - info.position.y);
		}
	};

	class CollidableTile
	{
	public:
		CollidableTile(const TileContent& content)
			:
			content(content)
		{
		}

		/*
			Return: repeated movement without physics,
			used for sliding or bouncing (normal walls slide too [ignores phyiscs])
		*/
		virtual sf::Vector2f onCollision(
			const CollisionType type,
			const Collision& collision) = 0;

		GameEvent<CollidableTile, 
			const CollisionType*, const Collision*
		> collisionEvent;

		const TileContent& content;

	protected:
		void notifyCollisionEvent(
			const CollisionType* type,
			const Collision* collision)
		{
			collisionEvent.notify(type, collision);
		}
	};
}
