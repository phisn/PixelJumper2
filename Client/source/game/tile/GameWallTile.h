#pragma once

#include <Client/source/game/tiletrait/CollidableTile.h>
#include <Client/source/game/tiletrait/StaticTile.h>

#include <Client/source/game/WorldBase.h>

#include <Client/source/resource/tiles/ResourceWall.h>


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

		static GameTileBase* Create(Resource::Tile* const tile)
		{
			Resource::WallTile* const resource = (Resource::WallTile*) tile->Content;

			if (tile->Header.contentSize != sizeof(Resource::WallTile::Content))
			{
				return NULL;
			}

			return new WallTile(
				resource->Content.density,
				resource->Content.friction,

				sf::Vector2f(
					tile->Header.x,
					tile->Header.y
				),
				sf::Vector2f(
					tile->Header.width,
					tile->Header.height
				)
			);
		}

		WallTile(
			const float density,
			const float drag,
			const sf::Vector2f position,
			const sf::Vector2f size)
			:
			StaticTile(
				COLOR,
				position,
				size),
			CollidableTile(10.f, 0.01f)
		{
		}

		void registerType(Environment* const env) override
		{
			CollidableTile::registerCollisionType(
				env,
				CollisionType::NormalCollision
			);
			StaticTile::registerType(env);
		}

		virtual sf::Vector2f onCollision(
			const CollisionType type,
			const Collision collision) override
		{
			CollidableTile::onCollision(type, collision);

			collision.player->state.position = collision.info.position;

			sf::Vector2f movement = { };
			sf::Vector2f remainMove = { };

			if (collision.info.isHorizontal())
			{
				movement.y = collision.player->state.readProperties()->movement.y;
				/*remainMove = WorldBase::ApplyFriction(
					&collision.player->state,
					drag,
					collision.timeValue,
					true,
					{
						0,
						(collision.target.y - collision.info.position.y)
					});*/

				remainMove.y = (collision.target.y - collision.info.position.y);
			}
			else
			{
				movement.x = collision.player->state.readProperties()->movement.x;
				/*remainMove = WorldBase::ApplyFriction(
					&collision.player->state,
					drag,
					collision.timeValue,
					true,
					{
						(collision.target.x - collision.info.position.x),
						0
					});*/

				remainMove.x = (collision.target.x - collision.info.position.x);
			}

			

			/*
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
			*/
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
	};
}
