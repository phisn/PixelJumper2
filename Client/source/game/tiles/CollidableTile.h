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
			public virtual Base
		{
		public:
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
		};
	}
}
