#pragma once

#include <Client/source/game/tiles/TileBase.h>
#include <Client/source/game/PlayerBase.h>

namespace Game
{
	namespace Tile
	{
		class Collidable
			:
			public virtual Base
		{
		public:
			bool checkCollision(
				const sf::Vector2f source,
				const sf::Vector2f destination,
				_Out_ sf::Vector2f* collision);

			virtual void onCollision(
				PlayerBase* player) = 0;
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
				_Out_ sf::Vector2f* collision);
		};
	}
}
