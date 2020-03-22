#pragma once

#include "CollisionEngine.h"
#include "GameEvent.h"
#include "GamePlayer.h"

#include <utility>
#include <vector>

namespace Game
{
	struct CollidableEvent
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

	struct CollidableTraitInfo
	{
		sf::Vector2f position;
		sf::Vector2f size;
	};

	struct CollidableTraitHandler
	{
		/*
			Return: repeated movement without physics,
			used for sliding or bouncing (normal walls slide too [ignores phyiscs])
		*/
		virtual sf::Vector2f onCollision(
			const CollisionType type,
			const CollidableEvent& collision) = 0;
	};

	struct CollidableTrait
	{
		CollidableTraitInfo info;
		CollidableTraitHandler* handler;
	};

	class CollidableTraitContainer
	{
	public:
		void pushCollidableTrait(
			const CollisionType collisionType,
			const CollidableTrait& collidable)
		{
			std::vector<CollidableTrait>& container = collidables[collisionType];

			if (container.size() == 0)
			{
				collisionTypes.push_back(collisionType);
			}

			container.push_back(collidable);
		}

		const std::vector<CollidableTrait>& findCollidableTrait(const CollisionType collisionType) const
		{
			decltype(collidables)::const_iterator iterator = collidables.find(collisionType);

			if (iterator == collidables.end())
			{
				return emptyContainer;
			}
			else
			{
				return iterator->second;
			}
		}

		const std::vector<CollisionType> getActiveCollisionTypes() const
		{
			return collisionTypes;
		}

	protected:
		std::vector<CollisionType> collisionTypes;
		std::unordered_map<
			CollisionType,
			std::vector<CollidableTrait>
		> collidables;

	private:
		std::vector<CollidableTrait> emptyContainer;
	};
}
