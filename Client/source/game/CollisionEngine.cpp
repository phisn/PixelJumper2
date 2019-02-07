#include "CollisionEngine.h"

#define OR_ENUM(e, v)  (int&) e |= (int) v
#define AND_ENUM(e, v) (int&) e &= (int) v

namespace
{
	Game::CollisionEngine::CollisionContext collisionContext;
	const sf::Vector2f defaultPlayerSize = { 1.f, 1.f };

	/*   __
  P1 -> *  * <- P2
		|  |
  P3 -> *__* <- P4
	*/

	struct PlayerCorner
	{
		char P1, P2, P3, P4;
	};


	/*     | G2
		   v
		 _____
		|     |
  G1 ->	|     |  <- G3
		|_____|
		   ^
		   | G4
	*/

	struct TileSides
	{
		char G1, G2, G3, G4;
	};

	struct CollisionOffsets
	{
		sf::Vector2f primary;
		sf::Vector2f secondary1, secondary2;
		int currentState = 0;
		// 1 = secondary1 used; 2 = secondary2 used
	};

	inline void pushNextEntryCollisionPoint(
		const int value,
		const sf::Vector2f offset,
		CollisionOffsets* const cp)
	{
		if (value == 2)
		{
			cp->primary = offset;
		}
		else
		{
			( cp->currentState
				? cp->secondary1
				: cp->secondary2
			) = offset;
		}
	}
}

namespace Game
{
	void CollisionEngine::SetCurrentContext(
		const CollisionEngine::CollisionContext cc)
	{
		collisionContext = cc;
	}

	bool Game::CollisionEngine::FindEnterCollision()
	{
		TileSides tileSides = { };
		PlayerCorner corner = { };

		if (collisionContext.position.x < collisionContext.destination.x)
		{
			++tileSides.G1;
			++corner.P2;
			++corner.P4;
		}
		else if(collisionContext.position.x > collisionContext.destination.x)
		{
			++tileSides.G3;
			++corner.P1;
			++corner.P3;
		}

		if (collisionContext.position.y < collisionContext.destination.x)
		{
			++tileSides.G2;
			++corner.P3;
			++corner.P4;
		}
		else if (collisionContext.position.y > collisionContext.destination.x)
		{
			++tileSides.G4;
			++corner.P1;
			++corner.P2;
		}

		if (*(int*) &tileSides == NULL)
		{
			return false;
		}

		CollisionOffsets collisionPoints = { };

		if (collisionContext.position.x == collisionContext.destination.x)
		{
			collisionPoints.primary = sf::Vector2f(
				collisionContext.position.x + defaultPlayerSize.x / 2.f,
				tileSides.G2 
				? collisionContext.position.y
				: collisionContext.position.y + collisionContext.tilesSize.y
			);
		}
		else if (collisionContext.position.y == collisionContext.destination.y)
		{
			collisionPoints.primary = sf::Vector2f(
				tileSides.G1
				? collisionContext.position.x
				: collisionContext.position.x + collisionContext.tilesSize.x,
				collisionContext.position.y + defaultPlayerSize.y / 2.f
			);
		}

		if (corner.P1)
		{
			pushNextEntryCollisionPoint(
				corner.P1,
				{ },
				&collisionPoints);
		}

		if (corner.P2)
		{
			pushNextEntryCollisionPoint(
				corner.P2,
				sf::Vector2f(
					defaultPlayerSize.x,
					0.f
				),
				&collisionPoints);
		}

		if (corner.P3)
		{
			pushNextEntryCollisionPoint(
				corner.P3,
				sf::Vector2f(
					0.f,
					defaultPlayerSize.y
				),
				&collisionPoints);
		}

		if (corner.P4)
		{
			pushNextEntryCollisionPoint(
				corner.P4, 
				sf::Vector2f(
					defaultPlayerSize.x,
					defaultPlayerSize.y
				),
				&collisionPoints);
		}



		return true;
	}

	bool Game::CollisionEngine::FindLeaveCollision()
	{

		return true;
	}
}
