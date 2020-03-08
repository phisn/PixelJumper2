#pragma once

#include "CollisionEngine.h"
#include "GameEvent.h"
#include "GamePlayer.h"

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
		CollidableTile()
		{
		}

		/*
			Return: repeated movement without physics,
			used for sliding or bouncing (normal walls slide too [ignores phyiscs])
		*/
		virtual sf::Vector2f onCollision(
			const CollisionType type,
			const Collision& collision) = 0;

		virtual sf::Vector2f getPosition() = 0;
		virtual sf::Vector2f getSize() = 0;
	};
}
