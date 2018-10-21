#pragma once

#include <Client/source/game/tiles/TileBase.h>
#include <Client/source/game/LocalPlayer.h>

namespace Game
{
	namespace Tile
	{
		/*
		
			Move collision to player (local)
			reasons:
			-	Allows more flex for tiles
			-	Easier impl for player colls
		
		*/

		struct Collision
		{
			enum Type
			{
				Vertical,
				Horizontal
			} type;

			sf::Vector2f position;
		};

		class Collidable
			:
			public Base
		{
		public:
			Collidable(const BaseSettings settings)
				:
				Base(settings)
			{
			}

			bool checkCollision(
				const sf::Vector2f source,
				const sf::Vector2f destination,
				_Out_ Collision* collision);

			virtual bool onCollision(
				const Collision collision,
				LocalPlayer* player) = 0;
		private:
			// return == ignore primary offset
			bool findOffsets(
				const sf::Vector2f source,
				const sf::Vector2f destination,
				_Out_ sf::Vector2f* primOffset,
				_Out_ sf::Vector2f* secOffset1,
				_Out_ sf::Vector2f* secOffset2);
			bool checkCollisionPath(
				const sf::Vector2f source,
				const sf::Vector2f destination,
				_Out_ Collision* collision);

			bool checkDefaultPath(
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

				if (t_w <= p_w_begin		&& t_w >= p_w_end							// 1. 
					&&
					t_y >= getPosition().y	&& t_y <= getPosition().y + getSize().y)	// 2.
				{
					collision->type = Collision::Type::Horizontal;
					collision->position = sf::Vector2f(
						t_w,
						t_y);

					return true;
				}

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

				if (t_h <= p_h_begin		&& t_h >= p_h_end							// 1.
					&&
					t_x >= getPosition().x	&& t_x <= getPosition().x + getSize().x)	// 2.
				{
					collision->type = Collision::Type::Vertical;
					collision->position = sf::Vector2f(
						t_x,
						t_h);

					return true;
				}

				// did not collide
				return false;
			}

			bool checkStraightPath(
				const sf::Vector2f source,
				const sf::Vector2f destination,
				_Out_ Collision* collision)
			{
				printf("Straight");

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

						return true;
					}
				}
				else
				{
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
						
						return true;
					}
				}

				return false;
			}
		};
	}
}
