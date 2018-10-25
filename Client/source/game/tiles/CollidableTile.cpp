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

		/*
		
			expected bug:
				offsets are not properbly working.
				assumption: only primary works
		
		*/

		sf::Vector2f
			primOffset,
			secOffset1,
			secOffset2;

		const bool result = findOffsets(
			source,
			destination,
			&primOffset,
			&secOffset1,
			&secOffset2);

		if (checkDefaultPath(
			source + primOffset,
			destination + primOffset,
			collision))
		{
			collision->position -= primOffset;

			return true;
		}

		if (result)
		{
			return false;
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

		if (source.x == destination.x)
		{
			primOffset->x = defaultTileSize / 2.f;
			primOffset->y = source.y < destination.y
				* defaultTileSize;

			return true;
		}

		if (source.y == destination.y)
		{
			primOffset->x = source.x < destination.x
				* defaultTileSize;
			primOffset->y = defaultTileSize / 2.f;

			return true;
		}

		if (source.x <= destination.x)
		{
			if (source.y <= destination.y)
			{
				/*   __
					+  +\ s1
					|  | \
				 s2 +__+
					\   \
					 \	 \ p
				*/

				secOffset1->x = defaultTileSize;
				secOffset1->y = 0.0f;

				secOffset2->x = 0.0f;
				secOffset2->y = defaultTileSize;

				primOffset->x = defaultTileSize;
				primOffset->y = defaultTileSize;

				return false;
			}

			if (source.y > destination.y)
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

				primOffset->x = defaultTileSize;
				primOffset->y = 0.0f;

				return false;
			}
		}

		if (source.x > destination.x)
		{
			if (source.y <= destination.y)
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

				primOffset->x = 0.0f;
				primOffset->y = defaultTileSize;

				return false;
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

				primOffset->x = 0.0f;
				primOffset->y = 0.0f;

				return false;
			}
		}

		// impossible
		return false;
	}
	
	bool Tile::Collidable::checkDefaultPath(
		const sf::Vector2f source, 
		const sf::Vector2f destination, 
		_Out_ Collision* collision)
	{
		if (source.x == destination.x ||
			source.y == destination.y)
		{
			return checkStraightPath(
				source,
				destination,
				collision);
		}

		/*

		g(x) = g_y = g_m * g_x + g_h

		g_m:
		P1(x1, y1),	    | y1 = g_m * x1 + g_h |
		P2(x2, y2)	  -	| y2 = g_m * x2 + g_h | :
		=> (y1 - y2) = g_m * x1 - g_m * x2
		=> (y1 - y2) = g_m * (x1 - x2)
		=> (y1 - y2) / (x1 - x2) = g_m
		g_h:
		=> y1 = g_m * x1 + g_h
		=> y1 - g_m * x1 = g_h

		*/

		const float g_m =
			(source.y - destination.y) /
			(source.x - destination.x);
		const float g_h = source.y - g_m * source.x;

		if (checkVerticalPath(
			source,
			destination,
			collision,
			g_m,
			g_h))
		{
			return true;
		}

		if (checkHorizontalPath(
			source,
			destination,
			collision,
			g_m,
			g_h))
		{
			return true;
		}

		return false;
	}
	
	bool Tile::Collidable::checkHorizontalPath(
		const sf::Vector2f source, 
		const sf::Vector2f destination, 
		_Out_ Collision* collision, 
		const float g_m, 
		const float g_h)
	{

		/*

		horizontal collision

		g(t_w) = t_y
		1.	=> t_w < p_w_begin && t_w > p_w_end
		2.	=> t_y < t_h_begin && t_y > t_h_end
		t_y:
		=> t_y = g_m * t_w + g_h

		*/

		const float t_w = source.x < destination.x
			? getPosition().x
			: getPosition().x + getSize().x;
		const float p_w_begin = source.x < destination.x
			? destination.x
			: source.x;
		const float p_w_end = source.x >= destination.x
			? destination.x
			: source.x;
		const float t_y = g_m * t_w + g_h;

		if (t_w <= p_w_begin && t_w >= p_w_end							// 1. 
			&&
			t_y >= getPosition().y	&& t_y <= getPosition().y + getSize().y)	// 2.
		{
			collision->type = Collision::Type::Horizontal;
			collision->position = sf::Vector2f(
				t_w,
				t_y);

			collision->remaining = destination.y - t_y;

			return true;
		}

		return false;
	}
	
	bool Tile::Collidable::checkVerticalPath(
		const sf::Vector2f source, 
		const sf::Vector2f destination,
		_Out_ Collision* collision, 
		const float g_m, 
		const float g_h)
	{
		/*

		vertical collision

		g(t_x) = t_h
		=> t_h < p_h_begin && t_h > p_h_end
		=> t_x < t_w_begin && t_x > t_w_begin
		t_x:
		=> t_h = g_m * t_x + g_h
		=> t_h - g_h = g_m * t_x
		=> t_x = (t_h - g_h) / g_m

		*/

		const float t_h = source.y < destination.y
			? getPosition().y
			: getPosition().y + getSize().y;
		const float p_h_begin = source.y < destination.y
			? destination.y
			: source.y;
		const float p_h_end = source.y >= destination.y
			? destination.y
			: source.y;
		const float t_x = (t_h - g_h) / g_m;

		if (t_h <= p_h_begin && t_h >= p_h_end							// 1.
			&&
			t_x >= getPosition().x	&& t_x <= getPosition().x + getSize().x)	// 2.
		{
			collision->type = Collision::Type::Vertical;
			collision->position = sf::Vector2f(
				t_x,
				t_h);

			collision->remaining = destination.x - t_x;

			return true;
		}

		return false;
	}
	
	bool Tile::Collidable::checkStraightPath(
		const sf::Vector2f source,
		const sf::Vector2f destination,
		_Out_ Collision* collision)
	{
		if (source.x == destination.x)
		{
			if (source.x < getPosition().x ||
				source.x > getPosition().x + getSize().x)
			{
				return false;
			}

			const float height = source.y < destination.y
				? getPosition().y
				: getPosition().y + getSize().y;

			const float y_begin = source.y > destination.y
				? source.y
				: destination.y;
			const float y_end = source.y <= destination.y
				? source.y
				: destination.y;

			if (y_begin >= height && y_end <= height)
			{
				collision->position = sf::Vector2f(
					source.x,
					height);
				collision->type = Collision::Type::Vertical;

				collision->remaining = 0.0f;

				return true;
			}
		}
		else
		{
			if (source.y < getPosition().y ||
				source.y > getPosition().y + getSize().y)
			{
				return false;
			}

			const float width = source.x < destination.x
				? getPosition().x
				: getPosition().x + getSize().x;

			const float x_begin = source.x > destination.x
				? source.x
				: destination.x;
			const float x_end = source.x <= destination.x
				? source.x
				: destination.x;

			if (x_begin >= width && x_end <= width)
			{
				collision->position = sf::Vector2f(
					width,
					source.y);
				collision->type = Collision::Type::Horizontal;

				collision->remaining = 0.0f;

				return true;
			}
		}

		return false;
	}
}
