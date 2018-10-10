#pragma once

#include <Client/source/game/tiles/CollidableTile.h>
#include <Client/source/game/tiles/TileBase.h>
#include <Client/source/game/tiles/TouchableTile.h>

#include <SFML/Graphics.hpp>

#include <vector>

namespace Game
{
	class World
	{
	public:
		World();
		~World();

		void onLogic(
			sf::Time time)
		{

		}

		bool validate() const
		{
			if (tiles.size() == 0)
			{
				return false;
			}

			// entries & exits > 0

			for (Tile::Base* root : tiles)
				for (Tile::Base* cold : tiles)
				{
					if (cold == root)
					{
						continue;
					}

					const float r_near_x = root->getPosition().x;
					const float r_far_x = r_near_x + root->getSize().x;

					const float c_near_x = cold->getPosition().x;
					const float c_far_x = c_near_x + cold->getSize().x;

					const float r_near_y = root->getPosition().y;
					const float r_far_y = r_near_x + root->getSize().y;

					const float c_near_y = cold->getPosition().y;
					const float c_far_y = c_near_y + cold->getSize().y;

					/*
					
						>= & <= only same nears and fars
						to solve a bug (Tiles on the same position)
					
					*/
					if ((		// x
							(		// 1. root x
							r_near_x >= c_near_x &&
							r_near_x < c_far_x 
								||	// 2. root x
							r_far_x > c_near_x &&
							r_far_x <= c_far_x
							) || (	// 1. cold x
							c_near_x >= r_near_x &&
							c_near_x < r_far_x
								||	// 2. cold x
							c_far_x > r_near_x &&
							c_far_x <= r_far_x
							)
						) && (	// y
							(		// 1. root y
							r_near_y >= c_near_y &&
							r_near_y < c_far_y
								||	// 2. root y
							r_far_y > c_near_y &&
							r_far_y <= c_far_y
							) || (	// 1. cold y
							c_near_y >= r_near_y &&
							c_near_y < r_far_y
								||	// 2. cold y
							c_far_y > r_near_y &&
							c_far_y <= r_far_y
							)
						))
					{
						return false;
					}
				}

			return true;
		}

		void draw() const
		{
			for (Tile::Base* tile : tiles)
			{
				tile->draw();
			}
		}
	private:
		std::vector<Tile::Base*> tiles;

		// entries & exits

		std::vector<Tile::Touchable*> touchable;
		std::vector<Tile::Collidable*> collidable;
	};
}
