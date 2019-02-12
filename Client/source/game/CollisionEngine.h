#pragma once

#include <SFML/System.hpp>

namespace Game
{
	namespace CollisionEngine
	{
		struct CollisionInfo
		{
			sf::Vector2f position;

			/*
			     G2
			     __
				+  +
			 G1	|  | G3
				+__+

			     G4
			*/

			enum Type
			{
				G1, G2, G3, G4
			} type;

			// if collision is vertical then the
			// remaining distance, is the distance
			// in the horizontal direction
			// with vertical, the other way around
			float remainingDistance;
		};

		struct CollisionContext
		{
			// collision function
			float G_M, G_H;

			// sorted position & target
			sf::Vector2f begin, end;

			// target position to determine
			// remaining distance in
			// collisionInformation
			sf::Vector2f target;

			// expected ways
			sf::Vector2f primaryOffset;
			sf::Vector2f secondary1, secondary2;

			// px == dx | py == dy
			bool isStraight;

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

		CollisionInfo GetLastCollision();
	}

	/*
	
		Usage:
			CollisionContext cc = CollisionEngine::SetupEnterCollisionContext(
				playerPosition,
				playerDestination);

			// would normaly do this for every tile
			bool collisionFound = CollisionEngine::FindEnterCollision(
				&cc,
				tileSize,
				tilePosition);
			if (collisionFound)
			{
				CollisionEngine::CollisionInformation ci = CollisionEngine::GetLastCollision();
				// use ci
			}
	*/
}
