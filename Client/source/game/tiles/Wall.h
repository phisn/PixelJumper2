#pragma once

#include <Client/source/game/tiles/CollidableTile.h>

namespace Game
{
	namespace Tile
	{
		class Wall
			:
			public Collidable
		{
		public:
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
