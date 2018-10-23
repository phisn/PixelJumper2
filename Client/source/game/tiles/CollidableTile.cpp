#include "CollidableTile.h"

namespace Game
{

	bool Tile::Collidable::checkCollision(
		const sf::Vector2f source,
		const sf::Vector2f destination,
		_Out_ Collision* collision)
	{
		if (source.x == destination.x &&
			source.y == destination.y)
		{
			return false;
		}

		/*
		
		extend with a quick check at beginning 
		if collision check makes sense
		# Ideas:
			- 
		
		*/

		sf::Vector2f 
			primOffset, 
			secOffset1, 
			secOffset2;

		if (!findOffsets(
				source,
				destination,
				&primOffset,
				&secOffset1,
				&secOffset2))
		{
			if (checkDefaultPath(
				source + primOffset,
				destination + primOffset,
				collision))
			{
				collision->position -= primOffset;

				return true;
			}
		}

		if (checkDefaultPath(
			source + secOffset1,
			destination + secOffset1,
			collision))
		{
			collision->position -= secOffset1;

			return true;
		}

		if (checkDefaultPath(
			source + secOffset2,
			destination + secOffset2,
			collision))
		{
			collision->position -= secOffset2;

			return true;
		}

		return false;
	}

	// return == ignore primary offset
	bool Tile::Collidable::findOffsets(
		const sf::Vector2f source, 
		const sf::Vector2f destination, 
		_Out_ sf::Vector2f* primOffset, 
		_Out_ sf::Vector2f* secOffset1, 
		_Out_ sf::Vector2f* secOffset2)
	{
		/*
		
			found a pattern in secOffset1 and secOffset2
			both repeat in 1 & 4 | 2 & 3. Primoffset does
			not repeat. possability for speed up
		
		*/

		const bool ignorePrimary =
			source.x == destination.x ||
			source.y == destination.y;
		// everything inverted
		if (source.x <= destination.x)
		{
			if (source.y <= destination.y)
			{
				/*   /s1
					/__ /p
					+  +
					|  | /s2
					+__+/
				*/


				secOffset1->x = 0.0f;
				secOffset1->y = 0.0f;

				secOffset2->x = defaultTileSize;
				secOffset2->y = defaultTileSize;

				if (ignorePrimary)
				{
					primOffset->x = defaultTileSize;
					primOffset->y = 0.0f;
				}

				return ignorePrimary;
			}

			if (source.y > destination.y)
			{
				/* p  \ s1
					\__\
				s2	+  +
				  \ |  |
				   \+__+
				*/


				secOffset1->x = defaultTileSize;
				secOffset1->y = 0.0f;

				secOffset2->x = 0.0f;
				secOffset2->y = defaultTileSize;

				if (ignorePrimary)
				{
					primOffset->x = 0.0f;
					primOffset->y = 0.0f;
				}

				return ignorePrimary;
			}
		}

		if (source.x > destination.x)
		{
			if (source.y <= destination.y)
			{
				/* p  \ s1
					\__\
				s2	+  +
				  \ |  |
				   \+__+
				*/

				secOffset1->x = 0.0f;
				secOffset1->y = 0.0f;

				secOffset2->x = defaultTileSize;
				secOffset2->y = defaultTileSize;

				if (ignorePrimary)
				{
					primOffset->x = defaultTileSize;
					primOffset->y = 0.0f;
				}

				return ignorePrimary;
			}

			if (source.y > destination.y)
			{
				/*    s1
					 __  p
				   /+  +
				  /	|  |  s2
					+__+
				   /   /
					  /
				*/

				secOffset1->x = defaultTileSize;
				secOffset1->y = 0.0f;

				secOffset2->x = 0.0f;
				secOffset2->y = defaultTileSize;

				if (ignorePrimary)
				{
					primOffset->x = defaultTileSize;
					primOffset->y = defaultTileSize;
				}

				return ignorePrimary;
			}
		}

		return false;
	}
}
