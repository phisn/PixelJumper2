#pragma once

#include <Client/source/game/tiles/CollidableTile.h>
#include <Client/source/game/tiles/TileBase.h>
#include <Client/source/game/tiles/TileManager.h>

namespace Game
{
	namespace Tile
	{
		struct WallSettings
		{
			sf::Vector2f position, size;
		};

		class Wall
			:
			public Collidable
		{
		public:
			Wall(
				const WallSettings settings)
				:
				Collidable(
					{
						settings.position,
						settings.size,

						Type::Collidable,
						sf::Color::White
					})
			{
			}

			~Wall()
			{
			}

			bool onCollision(
				const Collision collision,
				LocalPlayer* player,
				sf::Vector2f* remainingDestination)
			{
				if (collision.type == Collision::Vertical &&
					player->getPosition().y < collision.position.y)
				{
					player->changeMovement()->setOnGround(0.4f);
				}

				player->setPosition(
					collision.position);

				if (collision.type == Collision::Vertical)
				{
					player->changeMovement()->muliMovement(
						{ 1.f, 0.f });

					remainingDestination->x = collision.position.x + collision.remaining;
					remainingDestination->y = collision.position.y;
				}
				else
				{
					player->changeMovement()->muliMovement(
						{ 0.f, 1.f });

					remainingDestination->x = collision.position.x;
					remainingDestination->y = collision.position.y + collision.remaining;
				}

				return true;
			}
		};

		class WallResource
			:
			public BaseResource
		{
		public:
			static WallResource* registerWallResource()
			{
				
			}

			Base* create() override
			{
				return new Wall(settings);
			}

		private:
			WallSettings settings;
		};
	}
}
