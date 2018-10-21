#include "World.h"

namespace Game
{
	bool World::validate() const
	{
		if (settings->tiles.size() == 0)
		{
			return false;
		}

		// entries & exits > 0

		for (Tile::Base* root : settings->tiles)
			for (Tile::Base* cold : settings->tiles)
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
				return true;
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
}