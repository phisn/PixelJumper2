#pragma once

#include <Client/source/game/tiles/CollidableTile.h>
#include <Client/source/game/tiles/TileBase.h>

namespace Game
{
	namespace Tile
	{
		struct WallSettings // settings for binary file save / load
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
				LocalPlayer* player)
			{
				player->setPosition(
					collision.position);

				if (collision.type == Collision::Vertical)
				{
					player->changeMovement()->setOnGround(0.3f);

					player->changeMovement()->mutliMovement(
						{ 1.f, 0.f });
				}
				else
				{
					player->changeMovement()->mutliMovement(
						{ 0.f, 1.f });
				}

				return true;
			}
		};
	}
}
