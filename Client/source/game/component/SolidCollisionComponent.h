#pragma once

#include <Client/source/game/Environment.h>
#include <Client/source/game/tiletrait/CollidableTile.h>

namespace Game
{
	struct SolidCollisionContent
	{
		float density;
		float inputForceAddition;
		float friction;
	};

	class SolidCollisionComponent
		:
		public CollidableTile
	{
	public:
		SolidCollisionComponent(
			const TileContent& tileContent,
			const SolidCollisionContent content)
			:
			CollidableTile(tileContent),
			content(content)
		{
		}

		sf::Vector2f onCollision(
			const CollisionType type,
			const Collision& collision) override
		{
			collision.player->getProperties().position = collision.info.position;

			sf::Vector2f movement = {};
			sf::Vector2f remainOffset = {};

			if (collision.info.isHorizontal())
			{
				movement.y = collision.player->getProperties().movement->y;
				remainOffset.y = (collision.target.y - collision.info.position.y);

				if (fabsf(movement.y) < content.friction)
				{
					movement.y = 0;
				}
				else
				{
					movement.y += movement.y > 0 ? -content.friction : content.friction;
				}
			}
			else
			{
				movement.x = collision.player->getProperties().movement->x;
				remainOffset.x = (collision.target.x - collision.info.position.x);

				if (fabsf(movement.x) < content.friction)
				{
					movement.x = 0;
				}
				else
				{
					movement.x += movement.x > 0 ? -content.friction : content.friction;
				}
			}

			collision.player->getCollisionContainer().setCollider(
				collision.info.type,
				CollisionContainer::Collider{
					content.inputForceAddition,
					content.density
				});

			collision.player->getProperties().movement = movement;
			notifyCollisionEvent(&type, &collision);

			return remainOffset;
		}

		void registerComponent(Environment* const env)
		{
			env->registerCollisionType(
				Game::CollisionType::NormalCollision,
				this);
			env->registerTile<CollidableTile>(this);
		}

	private:
		const SolidCollisionContent content;
	};
}
