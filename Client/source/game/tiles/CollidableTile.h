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

		typedef BaseSettings CollidableSettings;

		struct Collision
		{
			enum Type
			{
				Vertical,
				Horizontal
			} type;

			sf::Vector2f position;
			float remaining = (float) true;
		};

		class Collidable
			:
			public Base
		{
		public:
			Collidable(
				const CollidableSettings settings)
				:
				Base(settings)
			{
			}

			virtual ~Collidable() { }

			bool checkCollision(
				const sf::Vector2f source,
				const sf::Vector2f destination,
				_Out_ Collision* collision);

			virtual bool onCollision(
				const Collision collision,
				LocalPlayer* player,
				sf::Vector2f* remainingDestination) = 0;
		private:
			// return == ignore primary offset
			bool findOffsets(
				const sf::Vector2f source,
				const sf::Vector2f destination,
				_Out_ sf::Vector2f* primOffset,
				_Out_ sf::Vector2f* secOffset1,
				_Out_ sf::Vector2f* secOffset2);

			bool checkDefaultPath(
				const sf::Vector2f source,
				const sf::Vector2f destination,
				_Out_ Collision* collision);

			bool checkHorizontalPath(
				const sf::Vector2f source,
				const sf::Vector2f destination,
				_Out_ Collision* collision,
				const float g_m,
				const float g_h);
			bool checkVerticalPath(
				const sf::Vector2f source,
				const sf::Vector2f destination,
				_Out_ Collision* collision,
				const float g_m,
				const float g_h);

			bool checkStraightPath(
				const sf::Vector2f source,
				const sf::Vector2f destination,
				_Out_ Collision* collision);
		};
	}
}
