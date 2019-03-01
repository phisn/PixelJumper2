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
			collision.player->position = collision.info.position;

			// sync has not to be forced, because this action does not 
			// do any significant independet change (goes to none [same on all machines])

			if (collision.info.isHorizontal())
			{
				collision.player->movement = 
				{ 
					0.f, collision.player->movement.getValue().y 
				};

				return 
				{ 
					0.0f, collision.target.y - collision.info.position.y 
				};
			}
			else
			{
				collision.player->movement =
				{
					collision.player->movement.getValue().x, 0.0f
				};

				return 
				{ 
					collision.target.x - collision.info.position.x, 0.0f 
				};
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
