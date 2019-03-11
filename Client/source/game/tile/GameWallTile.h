#pragma once

#include <Client/source/game/tiletrait/CollidableTile.h>
#include <Client/source/game/tiletrait/StaticTile.h>

#include <Client/source/game/WorldBase.h>




// test
#include <Client/source/game/LocalPlayer.h>

namespace Game
{
	class WallTile
		:
		public StaticTile,
		public CollidableTile
	{
	public:
		static const sf::Color COLOR;

		WallTile(
			const float density,
			const float drag,
			const sf::Vector2f position,
			const sf::Vector2f size)
			:
			StaticTile(
				sf::Color::White,
				position,
				size),
			drag(drag),
			CollidableTile(density)
		{
		}

		void registerType(Environment* const env) override
		{
			CollidableTile::registerCollisionType(
				env,
				CollisionType(
					false,
					false,
					false)
			);
			StaticTile::registerType(env);
		}

		virtual sf::Vector2f onCollision(
			const CollisionType type,
			const Collision collision) override
		{
			collision.player->state.position = collision.info.position;

			sf::Vector2f movement = { };
			sf::Vector2f remainMove = { };

			if (collision.info.isHorizontal())
			{
				movement.y = collision.player->state.readProperties()->movement.y;
				remainMove.y = (collision.target.y - collision.info.position.y)
					* WorldBase::CalculateFrictionLose(drag, collision.player->state.readProperties()->weight);
			}
			else
			{
				movement.x = collision.player->state.readProperties()->movement.x;
				remainMove.x = (collision.target.x - collision.info.position.x)
					* WorldBase::CalculateFrictionLose(drag, collision.player->state.readProperties()->weight);
			}

			if (collision.player->getType() == PlayerType::Local && collision.info.type == CollisionEngine::CollisionInfo::G3)
			{
				LocalPlayer* lp = (LocalPlayer*) collision.player;

				lp->movementRoutine.hook(
					[lp](const sf::Time time, const Direction direction)
					{
						if (direction == Direction::Up)
						{
							return;
						}

						if (direction == Direction::Down)
						{
							lp->movementRoutine.unhook();
						}

						lp->movementRoutine.getCore()(time, direction);
					});

			}

			collision.player->state.movement = movement;
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
