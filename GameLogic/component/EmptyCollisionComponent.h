#pragma once

#include "Environment.h"
#include "tiletrait/CollidableTile.h"

namespace Game
{
	class EmptyCollisionComponent
		:
		public CollidableTile
	{
	public:
		EmptyCollisionComponent(const TileContent& tileContent)
			:
			CollidableTile(tileContent)
		{
		}

		sf::Vector2f onCollision(
			const CollisionType type,
			const Collision& collision) override
		{
			sf::Vector2f result;

			if (collision.info.isHorizontal())
			{
				collision.player->getProperties().position =
				{
					collision.target.x,
					collision.info.position.y
				};

				result = { 0.f, collision.target.y - collision.info.position.y };
			}
			else
			{
				collision.player->getProperties().position =
				{
					collision.info.position.x,
					collision.target.y
				};

				result = { collision.target.x - collision.info.position.x, 0.f };
			}

			notifyCollisionEvent(&type, &collision);
			return result;
		}

		void registerComponent(
			Environment* const env,
			const CollisionType type)
		{
			env->registerCollisionType(type, this);
			env->registerTile<CollidableTile>(this);
		}

	};
}
