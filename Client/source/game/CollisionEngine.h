#pragma once

#include <SFML/System.hpp>

namespace Game
{
	struct CollisionType
	{
		CollisionType() = default;
		CollisionType(
			const bool isWeak,
			const bool invertPlayer,
			const bool invertTile)
			:
			isWeak(isWeak),
			invertPlayer(invertPlayer),
			invertTile(invertTile)
		{
		}

		const bool isWeak;

		const bool invertPlayer;
		const bool invertTile;
	private:
		char padding = 0;
	};

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

			bool isVertical() const
			{
				return type == G2 || type == G4;
			}

			bool isHorizontal() const
			{
				return !isVertical();
			}
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

			// Diagonal:
			// - use a single collision point
			//   instead of three
			// Straight:
			// - Do everything like normal
			bool isWeakCollision;

			// expected collision offsets
			// | | <- hSide
			//  =  <- vSide
			float vSideOffset, hSideOffset;
		};

		/*
		
			invertPosition:
			- t = false & p = false => Enter Collision
			- t = true  & p = true  => Leave Collision
		
		*/
		CollisionContext SetupCollisionContext(
			const sf::Vector2f position,
			const sf::Vector2f destination,
			const CollisionType type);

		/*
		static CollisionContext SetupEnterCollisionContext(
			const sf::Vector2f position,
			const sf::Vector2f destination)
		{
			return SetupCollisionContext(
				position,
				destination,
				false,
				false,
				false);
		}

		static CollisionContext SetupLeaveCollisionContext(
			const sf::Vector2f position,
			const sf::Vector2f destination)
		{
			return SetupCollisionContext(
				position,
				destination,
				true,
				true,
				true);
		}
		*/

		bool FindCollision(
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

static bool operator<(
	const Game::CollisionType& ct1,
	const Game::CollisionType& ct2)
{
	return *(int*)&ct1 < *(int*)&ct2;
}

static bool operator>(
	const Game::CollisionType& ct1,
	const Game::CollisionType& ct2)
{
	return *(int*)&ct1 > *(int*)&ct2;
}

static bool operator==(
	const Game::CollisionType& ct1,
	const Game::CollisionType& ct2)
{
	return *(int*)&ct1 == *(int*)&ct2;
}

