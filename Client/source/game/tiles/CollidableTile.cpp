#include "CollidableTile.h"

namespace Game
{

	bool Tile::Collidable::checkCollision(
		const sf::Vector2f source,
		const sf::Vector2f destination,
		_Out_ Collision* collision)
	{
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
			if (checkCollisionPath(
				source + primOffset,
				destination + primOffset,
				collision))
			{
				collision->position -= primOffset;

				return true;
			}
		}

		if (checkCollisionPath(
			source + secOffset1,
			destination + secOffset1,
			collision))
		{
			collision->position -= secOffset1;

			return true;
		}

		if (checkCollisionPath(
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
				/* p    s1
					 __
					+  +\
				s2  |  | \
					+__+
					\   \
					 \
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

				secOffset1->x = 0.0f;
				secOffset1->y = 0.0f;

				secOffset2->x = defaultTileSize;
				secOffset2->y = defaultTileSize;

				if (ignorePrimary)
				{
					primOffset->x = 0.0f;
					primOffset->y = defaultTileSize;
				}

				return ignorePrimary;
			}
		}
	}

	bool Tile::Collidable::checkCollisionPath(
		const sf::Vector2f source,
		const sf::Vector2f destination,
		_Out_ Collision* collision)
	{
		/*
		      t_h  ---\
             _____ <--/
			/     \
			|     | t_w
			|     |
			\_____/   |
			     /\   |
			      \---/
			\
			 \
			  \ g(x)
			   \
			    \
				 \
		
			g(x) = g_m * x + g_h

			1. g(x) = t_h => x > t_w_a && x < t_w_b
				=> (t_h - g_h) / g_m = x => x > t_w_a && x < t_w_b
			2. g(t_w) = y => y > t_h_a && y < t_h_b
				=> g_m * t_w + g_h = x => x > t_h_a && x < t_h_b

			1. g(x) = t_h
				=> g_m * x + g_h = t_h
				=> (t_h - g_h) / g_m = x
		
		*/

		/* 
		
		summarize everything and try to find faster
		way to calculate using CAS. possability for speed up

		*/ 

		const float g_m =
			(source.x - destination.x) /
			(source.y - destination.y);
		const float g_h = source.x - g_m * source.y;

		const float t_h_a = shape.getPosition().y;
		const float t_h_b = t_h_a + shape.getSize().y;

		const float t_w_a = shape.getPosition().x;
		const float t_w_b = t_w_a + shape.getSize().x;

		const float t_h = g_m > 0.0f 
			? t_h_b : t_h_a;
		const float t_w = source.x > destination.x
			? t_w_b : t_w_a;

		if (source.x != destination.x)
		{ // ignore if x does not change

			/*   ___
			   a+   +
			   _|___|_ <- g(x)
				|   |
			   b+___+
				/\
				 \ t_w
			*/

			// 2. g(t_w) = y = > y > t_h_a && y < t_h_b
			//	= > g_m * t_w + g_h = x = > x > t_h_a && x < t_h_b
			const float y = g_m * t_w + g_h;

			if (y > t_h_a && y < t_h_b)
			{ // on collision horizontal
				collision->position.x = t_w;
				collision->position.y = y;

				collision->type = Collision::Horizontal;

				return true;
			}
		}
		else if (source.y != destination.y) // special case
		{
			const float _t_h = source.y > destination.y
				? t_h_a
				: t_h_b;
			const float _p_h_a = source.y > destination.y
				? source.y
				: destination.y;
			const float _p_h_b = source.y < destination.y
				? source.y
				: destination.y;

			if (source.x > t_w_a && source.x < t_w_b &&
				_p_h_a > _t_h && _p_h_b < _t_h)
			{
				collision->position.x = source.x;
				collision->position.y = _t_h;

				collision->type = Collision::Vertical;

				return true;
			}
		}

		if (source.y != destination.y)
		{ // ignore if y does not change
			/*    | <- g(x)
				 ___  <- t_h
				+ | + 
			    | | |
				| | |
				+_|_+ 
				a | b
 			*/


			// 1. g(x) = t_h = > x > t_w_a && x < t_w_b
			// 	=> (t_h - g_h) / g_m = x = > x > t_w_a && x < t_w_b
			const float x = (t_h - g_h) / g_m;

			if (x > t_w_a && x < t_w_b)
			{ // on collision vertical
				collision->position.x = x;
				collision->position.y = t_h;

				collision->type = Collision::Vertical;

				return true;
			}
		}

		return false;
	}
}
