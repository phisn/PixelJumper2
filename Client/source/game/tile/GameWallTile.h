#pragma once

#include <Client/source/game/tiletrait/CollidableTile.h>
#include <Client/source/game/tiletrait/StaticTile.h>

namespace Game
{
	class WallTile
		:
		public StaticTile,
		public CollidableTile
	{
	public:
		WallTile(
			const sf::Vector2f position,
			const sf::Vector2f size)
			:
			StaticTile(
				sf::Color::White,
				position,
				size)
		{
		}

		void initialize(TileContainer* const container) override
		{
			CollidableTile::addCollisionType(
				container,
				CollisionType(
					false,
					false,
					false)
			);
			StaticTile::initialize(container);
		}

		sf::Vector2f onCollision(
			const CollisionType type,
			const Collision collision) override
		{
			collision.player->Properties.position = collision.info.position;

			// sync has not to be forces, because this action does not 
			// do any significant independet change (goes to none [same on all machines])

			if (collision.info.isHorizontal())
			{
				collision.player->Properties.movement.x = 0.0f;

				return { 0.0f, collision.target.y - collision.info.position.y };
			}
			else
			{
				collision.player->Properties.movement.y = 0.0f;

				return { collision.target.x - collision.info.position.x, 0.0f };
			}
		}

		const sf::Vector2f getPosition() const override
		{
			return StaticTile::getPosition();
		}

		const sf::Vector2f getSize() const override
		{
			return StaticTile::getSize();
		}
	};
}
