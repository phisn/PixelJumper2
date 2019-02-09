#include "CollisionEngine.h"

namespace
{
	const sf::Vector2f currentPlayerSize = { 1.f, 1.f };
	Game::CollisionEngine::CollisionInformation lastCollision;

	/*   __
  P1 -> *  * <- P2
		|  |
  P3 -> *__* <- P4
	*/

	struct PlayerCorner
	{
		char P1, P2, P3, P4;
	};

	inline void pushNextEntryCollisionPoint(
		const sf::Vector2f offset,
		const int value,
		Game::CollisionEngine::CollisionContext* const cp)
	{
		if (value == 2) // incremented 2x == primary corner
		{
			cp->primaryOffset = offset;
		}
		else
		{
			// ABUSING G_M to indicate if
			// secondary1 is already used
			( cp->G_M
				? cp->secondary2
				: cp->secondary1
			) = offset;

			cp->G_M = (float) true;
		}
	}

	inline bool FindHorizontalCollisionInPath(
		const Game::CollisionEngine::CollisionContext* const collisionContext,
		const sf::Vector2f tileSize,
		const sf::Vector2f tilePosition)
	{
		/*

		horizontal collision

		g(t_w) = t_y
		1.	=> t_w < p_w_begin && t_w > p_w_end
		2.	=> t_y < t_h_begin && t_y > t_h_end
		t_y:
		=> t_y = g_m * t_w + g_h

		*/

		const float t_w = collisionContext->hSideOffset
			? tilePosition.x + tileSize.x
			: tilePosition.x;
		const float t_y =
			collisionContext->G_M * t_w + collisionContext->G_H;

		if (t_w <= collisionContext->begin.x &&
			t_w >= collisionContext->end.x
			&&
			t_y >= tilePosition.y &&
			t_y <= tilePosition.y + tileSize.y)
		{
			// set value
			// position: x = t_w; y = t_y
			lastCollision.position.x = t_w;
			lastCollision.position.y = t_y;
			
			lastCollision.type = Game::CollisionEngine::CollisionInformation::Horizontal;

			lastCollision.remainingDistance = collisionContext->target.y - t_y;

			return true;
		}

		return false;
	}

	inline bool FindVerticalCollisionInPath(
		const Game::CollisionEngine::CollisionContext* const collisionContext,
		const sf::Vector2f tileSize,
		const sf::Vector2f tilePosition)
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

		const float t_h = collisionContext->vSideOffset
			? tilePosition.y + tileSize.y
			: tilePosition.y;
		const float t_x = 
			(t_h - collisionContext->G_H) / collisionContext->G_M;

		if (t_h <= collisionContext->begin.y &&
			t_h >= collisionContext->end.y
			&&
			t_x >= tilePosition.x &&
			t_x <= tilePosition.x + tileSize.x)
		{
			// set value
			// position: x = t_x; y = t_h
			lastCollision.position.x = t_x;
			lastCollision.position.y = t_h;

			lastCollision.type = Game::CollisionEngine::CollisionInformation::Vertical;

			lastCollision.remainingDistance = collisionContext->target.y - t_x;

			return true;
		}
		
		return false;
	}
	
	inline bool FindStraightHorizontalCollisionInPath(
		const Game::CollisionEngine::CollisionContext* const collisionContext,
		const sf::Vector2f tileSize,
		const sf::Vector2f tilePosition)
	{
		const float w = collisionContext->hSideOffset
			? tilePosition.x + tileSize.x
			: tilePosition.x;

		if (collisionContext->begin.x >= w &&
			collisionContext->end.x <= w)
		{
			// position.y == target.y == begin.y == end.y
			lastCollision.position.x = w;
			lastCollision.position.y = collisionContext->begin.y;

			lastCollision.type = Game::CollisionEngine::CollisionInformation::Horizontal;

			lastCollision.remainingDistance = 0.f;

			return true;
		}

		return false;
	}

	inline bool FindStraightVerticalCollisionInPath(
		const Game::CollisionEngine::CollisionContext* const collisionContext,
		const sf::Vector2f tileSize,
		const sf::Vector2f tilePosition)
	{
		const float h = collisionContext->vSideOffset
			? tilePosition.y + tileSize.y
			: tilePosition.y;

		if (collisionContext->begin.y >= h &&
			collisionContext->end.y <= h)
		{
			// position.x == target.x == begin.x == end.x
			lastCollision.position.x = collisionContext->begin.x;
			lastCollision.position.y = h;

			lastCollision.type = Game::CollisionEngine::CollisionInformation::Vertical;

			lastCollision.remainingDistance = 0.f;

			return true;
		}

		return false;
	}

	bool FindCollisionInPath(
		const Game::CollisionEngine::CollisionContext* const collisionContext,
		const sf::Vector2f tileSize,
		const sf::Vector2f tilePosition)
	{
		if (collisionContext->isStraight)
		{
			// G_M is abused to indicate if
			// is horizontal == 1 or vertical == 0
			if (collisionContext->G_M)
			{
				return FindStraightHorizontalCollisionInPath(
					collisionContext,
					tileSize,
					tilePosition);
			}
			else
			{
				return FindStraightVerticalCollisionInPath(
					collisionContext,
					tileSize,
					tilePosition);
			}
		}

		if (FindVerticalCollisionInPath(
			collisionContext,
			tileSize,
			tilePosition))
		{
			return true;
		}

		if (FindHorizontalCollisionInPath(
			collisionContext,
			tileSize,
			tilePosition))
		{
			return true;
		}

		return false;
	}
}

namespace Game
{
	CollisionEngine::CollisionContext CollisionEngine::SetupEnterCollisionContext(
		const sf::Vector2f position, 
		const sf::Vector2f destination)
	{
		CollisionContext result = { };
		PlayerCorner corner = { };

		// find used corners & offsets
		if (position.x < destination.x)
		{
			result.hSideOffset = false;
			++corner.P2;
			++corner.P3;
		}
		else if (position.x > destination.x)
		{
			result.hSideOffset = true;
			++corner.P1;
			++corner.P4;
		}

		if (position.y < destination.y)
		{
			result.vSideOffset = false;
			++corner.P3;
			++corner.P4;
		}
		else if (position.y > destination.y)
		{
			result.vSideOffset = true;
			++corner.P1;
			++corner.P2;
		}

		// prepush primary collision point if
		// movement is straight
		if (position.x == destination.x) // vertical
		{
			result.isStraight = true;

			result.primaryOffset = sf::Vector2f(
				position.x + currentPlayerSize.x / 2.f,
				corner.P1 || corner.P2 // == posy > desy
				? position.y
				: position.y + currentPlayerSize.y
			);

			// abuse G_M to indicate vertical movement
			result.G_M = (float) false;
		}
		else if (position.y == destination.y) // horizontal
		{
			result.isStraight = true;

			result.primaryOffset = sf::Vector2f(
				corner.P1 || corner.P4 // == posx > desx
				? position.x
				: position.x + currentPlayerSize.x,
				position.y + currentPlayerSize.y / 2.f
			);

			// abuse G_M to indicate horizontal movement
			result.G_M = (float) true;
		}

		// abusing G_M in pushNextEntryCollisionPoint
		// calculate to be used collision points
		if (corner.P1)
		{
			pushNextEntryCollisionPoint(
				{ },
				corner.P1,
				&result);
		}

		if (corner.P2)
		{
			pushNextEntryCollisionPoint(
				sf::Vector2f(
					currentPlayerSize.x,
					0.f),
				corner.P1,
				&result);
		}

		if (corner.P3)
		{
			pushNextEntryCollisionPoint(
				sf::Vector2f(
					currentPlayerSize.x,
					0.f),
				corner.P1,
				&result);
		}

		if (corner.P4)
		{
			pushNextEntryCollisionPoint(
				sf::Vector2f(
					currentPlayerSize.x,
					currentPlayerSize.y),
				corner.P1,
				&result);
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

		if (!result.isStraight) // skip for straight (division by zero)
		{
			result.G_M =
				(position.y - destination.y) /
				(position.x - destination.y);
			result.G_H =
				position.y - result.G_M * position.x;
		}
		
		// sort by x / y
		if (position.x < destination.x)
		{
			result.begin.x = destination.x;
			result.end.x = position.x;
		}
		else
		{
			result.begin.x = position.x;
			result.end.x = destination.x;
		}

		if (position.y < destination.y)
		{
			result.begin.y = destination.y;
			result.end.y = position.y;
		}
		else
		{
			result.begin.y = position.y;
			result.end.y = destination.y;
		}

		return result;
	}
	
	CollisionEngine::CollisionContext CollisionEngine::SetupLeaveCollisionContext(
		const sf::Vector2f position,
		const sf::Vector2f destination)
	{
		CollisionContext result = { };
		PlayerCorner corner = { };

		// find used corners & offsets
		if (position.x < destination.x)
		{
			result.hSideOffset = true;
			++corner.P1;
			++corner.P4;
		}
		else if (position.x > destination.x)
		{
			result.hSideOffset = false;
			++corner.P2;
			++corner.P3;
		}

		if (position.y < destination.y)
		{
			result.vSideOffset = true;
			++corner.P1;
			++corner.P2;
		}
		else if (position.y > destination.y)
		{
			result.vSideOffset = false;
			++corner.P3;
			++corner.P4;
		}

		// prepush primary collision point if
		// movement is straight
		if (position.x == destination.x) // vertical
		{
			result.isStraight = true;

			result.primaryOffset = sf::Vector2f(
				position.x + currentPlayerSize.x / 2.f,
				corner.P1 || corner.P2 // == posy < desy
				? position.y + currentPlayerSize.y
				: position.y
			);

			// abuse G_M to indicate vertical movement
			result.G_M = (float)false;
		}
		else if (position.y == destination.y) // horizontal
		{
			result.isStraight = true;

			result.primaryOffset = sf::Vector2f(
				corner.P1 || corner.P4 // == posx < desx
				? position.x
				: position.x + currentPlayerSize.x,
				position.y + currentPlayerSize.y / 2.f
			);

			// abuse G_M to indicate horizontal movement
			result.G_M = (float)true;
		}

		if (result.isStraight) // straight has normal 3 points
		{
			// abusing G_M in pushNextEntryCollisionPoint
			// calculate to be used collision points
			if (corner.P1)
			{
				pushNextEntryCollisionPoint(
					{ },
					corner.P1,
					&result);
			}

			if (corner.P2)
			{
				pushNextEntryCollisionPoint(
					sf::Vector2f(
						currentPlayerSize.x,
						0.f),
					corner.P1,
					&result);
			}

			if (corner.P3)
			{
				pushNextEntryCollisionPoint(
					sf::Vector2f(
						currentPlayerSize.x,
						0.f),
					corner.P1,
					&result);
			}

			if (corner.P4)
			{
				pushNextEntryCollisionPoint(
					sf::Vector2f(
						currentPlayerSize.x,
						currentPlayerSize.y),
					corner.P1,
					&result);
			}
		}
		else // diagonal has only a single (primary) point
		{
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

			result.G_M =
				(position.y - destination.y) /
				(position.x - destination.y);
			result.G_H =
				position.y - result.G_M * position.x;
		}

		// sort by x / y
		if (position.x < destination.x)
		{
			result.begin.x = destination.x;
			result.end.x = position.x;
		}
		else
		{
			result.begin.x = position.x;
			result.end.x = destination.x;
		}

		if (position.y < destination.y)
		{
			result.begin.y = destination.y;
			result.end.y = position.y;
		}
		else
		{
			result.begin.y = position.y;
			result.end.y = destination.y;
		}

		return result;
	}

	bool CollisionEngine::FindEnterCollision(
		const CollisionContext* const collisionContext, 
		const sf::Vector2f tileSize, 
		const sf::Vector2f tilePosition)
	{
		// instead of adding playerTile offsets to
		// all player calulations, we just substract it 
		// from the tile position -> easier and faster
		if (FindCollisionInPath(
				collisionContext,
				tileSize,
				tilePosition - collisionContext->primaryOffset))
		{
			return true;
		}

		if (FindCollisionInPath(
				collisionContext,
				tileSize,
				tilePosition - collisionContext->secondary1))
		{
			return true;
		}

		if (FindCollisionInPath(
				collisionContext,
				tileSize,
				tilePosition - collisionContext->secondary2))
		{
			return true;
		}

		return false;
	}

	bool CollisionEngine::FindLeaveCollision(
		const CollisionContext* const collisionContext, 
		const sf::Vector2f tileSize, 
		const sf::Vector2f tilePosition)
	{
		if (collisionContext->isStraight)
		{
			// abuse findentercollision because
			// of identical function
			return FindEnterCollision(
				collisionContext,
				tileSize,
				tilePosition
			);
		}
		else
		{
			return FindCollisionInPath(
				collisionContext,
				tileSize,
				tilePosition - collisionContext->primaryOffset
			);
		}
	}

	CollisionEngine::CollisionInformation CollisionEngine::GetLastCollision()
	{
		return lastCollision;
	}
}
