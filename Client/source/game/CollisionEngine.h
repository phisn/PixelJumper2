#pragma once

#include <SFML/System.hpp>

namespace Game
{
	namespace CollisionEngine
	{
		struct CollisionContext
		{
			// collision function
			float G_M, G_H;

			// sorted position & target
			sf::Vector2f begin, end;

			// expected ways
			sf::Vector2f primaryOffset;
			sf::Vector2f secondary1, secondary2;

			// px == dx | py == dy
			bool isDiagonal;

			// expected collision offsets
			// | | <- hSide
			//  =  <- vSide
			float vSideOffset, hSideOffset;
		};

		CollisionContext SetupEnterCollisionContext(
			const sf::Vector2f position,
			const sf::Vector2f destination);
		CollisionContext SetupLeaveCollisionContext(
			const sf::Vector2f position,
			const sf::Vector2f destination);

		bool FindEnterCollision(
			const CollisionContext* const collisionContext,
			const sf::Vector2f tileSize,
			const sf::Vector2f tilePosition);
		bool FindLeaveCollision(
			const CollisionContext* const collisionContext,
			const sf::Vector2f tileSize,
			const sf::Vector2f tilePosition);
	}
}
