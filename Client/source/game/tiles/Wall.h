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
			public Base,
			public Collidable
		{
		public:
			Wall(
				const WallSettings settings)
				:
				Base({
						settings.position, 
						settings.size,
						
						Type::Collidable,
						sf::Color::White
					}),
				Collidable(&shape)
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
					player->changeProperties()->isOnGround = true;

					player->changeProperties()->movement = sf::Vector2f(
						player->getProperties()->movement.x, 
						0.f);
				}
				else
				{
					player->changeProperties()->movement = sf::Vector2f(
						0.f,
						player->getProperties()->movement.x);
				}

				return true;
			}
		};
	}
}
