#pragma once

#include <Client/source/game/Simulator.h>

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
			const float drag,
			const sf::Vector2f position,
			const sf::Vector2f size)
			:
			StaticTile(
				sf::Color::White,
				position,
				size),
			drag(drag)
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

		virtual sf::Vector2f onCollision(
			const CollisionType type,
			const Collision collision) override
		{
			collision.player->position = collision.info.position;

			sf::Vector2f movement = { };
			sf::Vector2f remainMove = { };

			if (collision.info.isHorizontal())
			{
				movement.y = collision.player->movement.getValue().y;
				remainMove.y = (collision.target.y - collision.info.position.y)
					* _N_Simulator::CalculateMovementLose(drag, collision.player->weight);
			}
			else
			{
				movement.x = collision.player->movement.getValue().x;
				remainMove.x = (collision.target.x - collision.info.position.x)
					* _N_Simulator::CalculateMovementLose(drag, collision.player->weight);
			}

			collision.player->movement = movement;
			return remainMove;
		}

		const sf::Vector2f getPosition() const override
		{
			return StaticTile::getPosition();
		}

		const sf::Vector2f getSize() const override
		{
			return StaticTile::getSize();
		}
	private:
		const float drag;
	};
}
