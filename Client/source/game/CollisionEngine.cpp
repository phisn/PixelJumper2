#include "CollisionEngine.h"
#include <Client/source/logger/Logger.h>

#define PRECISION 1'000

namespace
{
	const sf::Vector2f currentPlayerSize = { 1.f, 1.f };
	Game::CollisionEngine::CollisionInfo lastCollision;

	/*   __
  P1 -> *  * <- P2
		|  |
  P3 -> *__* <- P4
	*/

	struct PlayerCorner
	{
		char P1, P2, P3, P4;
	};

	inline bool FindHorizontalCollisionInPath(
		const Game::CollisionEngine::CollisionContext* const collisionContext,
		const sf::Vector2f tileSize,
		const sf::Vector2f tilePosition,
		const bool isWeak)
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

		if (isWeak
			? t_w < collisionContext->begin.x &&
			  t_w > collisionContext->end.x
			  &&
			  t_y > tilePosition.y &&
			  t_y < tilePosition.y + tileSize.y

			: t_w <= collisionContext->begin.x &&
  			  t_w >= collisionContext->end.x
			  &&
			  t_y >= tilePosition.y &&
			  t_y <= tilePosition.y + tileSize.y)
		{
			// set value
			// position: x = t_w; y = t_y
			lastCollision.position.x = t_w;
			lastCollision.position.y = std::roundf(t_y * PRECISION) / PRECISION;;

			lastCollision.type = collisionContext->hSideOffset
				? Game::CollisionEngine::CollisionInfo::G3
				: Game::CollisionEngine::CollisionInfo::G1;

			return true;
		}

		return false;
	}

	inline bool FindVerticalCollisionInPath(
		const Game::CollisionEngine::CollisionContext* const collisionContext,
		const sf::Vector2f tileSize,
		const sf::Vector2f tilePosition,
		const bool isWeak)
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

		if (isWeak
			? t_h < collisionContext->begin.y &&
			  t_h > collisionContext->end.y
			  &&
			  t_x > tilePosition.x &&
			  t_x < tilePosition.x + tileSize.x

			: t_h <= collisionContext->begin.y &&
			  t_h >= collisionContext->end.y
			  &&
			  t_x >= tilePosition.x &&
			  t_x <= tilePosition.x + tileSize.x)
		{
			// set value
			// position: x = t_x; y = t_h
			lastCollision.position.x = std::roundf(t_x * PRECISION) / PRECISION;
			lastCollision.position.y = t_h;

			lastCollision.type = collisionContext->vSideOffset
				? Game::CollisionEngine::CollisionInfo::G4
				: Game::CollisionEngine::CollisionInfo::G2;

			return true;
		}
		
		return false;
	}
	
	inline bool FindStraightHorizontalCollisionInPath(
		const Game::CollisionEngine::CollisionContext* const collisionContext,
		const sf::Vector2f tileSize,
		const sf::Vector2f tilePosition,
		const bool isWeak)
	{
		const float h = collisionContext->vSideOffset
			? tilePosition.y + tileSize.y
			: tilePosition.y;
		const float w = collisionContext->hSideOffset
			? tilePosition.x + tileSize.x
			: tilePosition.x;

		if (collisionContext->begin.x >= w && collisionContext->end.x <= w
			&& (isWeak
			? collisionContext->begin.y > tilePosition.y
			  && 
			  collisionContext->begin.y < tilePosition.y + tileSize.y
			: collisionContext->begin.y >= tilePosition.y
			  && 
			  collisionContext->begin.y <= tilePosition.y + tileSize.y
			))
		{
			// position.y == target.y == begin.y == end.y
			lastCollision.position.x = w;
			lastCollision.position.y = collisionContext->begin.y;

			lastCollision.type = collisionContext->hSideOffset
				? Game::CollisionEngine::CollisionInfo::G3
				: Game::CollisionEngine::CollisionInfo::G1;

			return true;
		}

		return false;
	}

	inline bool FindStraightVerticalCollisionInPath(
		const Game::CollisionEngine::CollisionContext* const collisionContext,
		const sf::Vector2f tileSize,
		const sf::Vector2f tilePosition,
		const bool isWeak)
	{
		const float h = collisionContext->vSideOffset
			? tilePosition.y + tileSize.y
			: tilePosition.y;
		const float w = collisionContext->hSideOffset
			? tilePosition.x + tileSize.x
			: tilePosition.x;

		if (collisionContext->begin.y >= h && collisionContext->end.y <= h
			&& (isWeak
			? collisionContext->begin.x > tilePosition.x 
			  && 
			  collisionContext->begin.x < tilePosition.x + tileSize.x
			: collisionContext->begin.x >= tilePosition.x
  			  && 
			  collisionContext->begin.x <= tilePosition.x + tileSize.x
			))
		{
			// position.x == target.x == begin.x == end.x
			lastCollision.position.x = collisionContext->begin.x;
			lastCollision.position.y = h;

			lastCollision.type = collisionContext->vSideOffset
				? Game::CollisionEngine::CollisionInfo::G4
				: Game::CollisionEngine::CollisionInfo::G2;

			return true;
		}

		return false;
	}



	bool FindCollisionInPath(
		const Game::CollisionEngine::CollisionContext* const collisionContext,
		const sf::Vector2f tileSize,
		const sf::Vector2f tilePosition,
		const bool isWeak)
	{
		if (collisionContext->isStraight)
		{
			// G_M is abused to indicate if
			// is horizontal == 1 or vertical == 0
			if ((bool) collisionContext->G_M)
			{
				return FindStraightHorizontalCollisionInPath(
					collisionContext,
					tileSize,
					tilePosition,
					isWeak);
			}
			else
			{
				return FindStraightVerticalCollisionInPath(
					collisionContext,
					tileSize,
					tilePosition,
					isWeak);
			}
		}

		return FindVerticalCollisionInPath(
				collisionContext,
				tileSize,
				tilePosition,
				false)
			||
			FindHorizontalCollisionInPath(
				collisionContext,
				tileSize,
				tilePosition,
				false);
	}
}

namespace Game
{
	CollisionType CollisionType::NormalCollision{ false, false, false };
	CollisionType CollisionType::LeaveCollision{ true, true, true };

	CollisionEngine::CollisionContext CollisionEngine::SetupCollisionContext(
		const sf::Vector2f position, 
		const sf::Vector2f destination, 
		const CollisionType type)
	{
		CollisionContext result = { };
		PlayerCorner corner = { };

		result.isWeakCollision = type.isWeak;

		// determine tile offsets
		result.hSideOffset = !type.invertTile ^ (position.x < destination.x);
		result.vSideOffset = !type.invertTile ^ (position.y < destination.y);

		// prepare for futher compare
		const bool xCmpPlayerResult = type.invertPlayer ^ (position.x < destination.x);
		const bool yCmpPlayerResult = type.invertPlayer ^ (position.y < destination.y);

		// prepush primary collision point if
		// movement is straight
		if (position.x == destination.x) // vertical
		{
			// abuse G_M to indicate vertical movement
			result.G_M = (float) false;
			result.isStraight = true;

			result.primaryOffset =
			{
				currentPlayerSize.x / 2.f,
				yCmpPlayerResult ? currentPlayerSize.y : 0
			};

			result.secondary1 =
			{
				yCmpPlayerResult ? currentPlayerSize.x : 0,
				yCmpPlayerResult ? currentPlayerSize.y : 0,
			};

			result.secondary2 =
			{
				yCmpPlayerResult ? 0 : currentPlayerSize.x,
				yCmpPlayerResult ? currentPlayerSize.y : 0,
			};
		}
		else if (position.y == destination.y) // horizontal
		{
			// abuse G_M to indicate horizontal movement
			result.G_M = (float) true;
			result.isStraight = true;

			result.primaryOffset =
			{
				xCmpPlayerResult ? currentPlayerSize.x : 0,
				currentPlayerSize.y / 2.f
			};

			result.secondary1 =
			{
				xCmpPlayerResult ? currentPlayerSize.x : 0,
				xCmpPlayerResult ? currentPlayerSize.y : 0,
			};

			result.secondary2 =
			{
				xCmpPlayerResult ? currentPlayerSize.x : 0,
				xCmpPlayerResult ? 0 : currentPlayerSize.y,
			};
		}
		else
		{
			if (!type.isWeak)
			{
				/*
					A = px < dx
					B = py < dy

					A B	=>   prim      sec1      sec2
					0 0 => { 0, 0 }, { 0, 1 }, { 1, 0 }
					0 1 => { 0, 1 }, { 0, 0 }, { 1, 1 }
					1 0 => { 1, 0 }, { 0, 0 }, { 1, 1 }
					1 1 => { 1, 1 }, { 0, 1 }, { 1, 0 }

					-----------------------------------

						prim = { A,      B }
						sec1 = { 0,  A ^ B }
						sec2 = { 1, !A ^ B }
					
					-----------------------------------
				*/

				result.secondary1 =
				{
					0.f,
					xCmpPlayerResult ^ yCmpPlayerResult
					? 0.f
					: currentPlayerSize.y
				};

				result.secondary2 =
				{
					currentPlayerSize.x,
					xCmpPlayerResult ^ yCmpPlayerResult
					? currentPlayerSize.y
					: 0.f
				};
			}

			result.primaryOffset =
			{
				xCmpPlayerResult ? currentPlayerSize.x : 0,
				yCmpPlayerResult ? currentPlayerSize.y : 0
			};


			/*
				(Only for diagonal movement needed / possible)
				Caluclating function to
				represent player movement

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
				(position.x - destination.x);
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

		/*
		Log::Information(L"Begin x: " + std::to_wstring(result.begin.x));
		Log::Information(L"Begin y: " + std::to_wstring(result.begin.y));
		Log::Information(L"End x: " + std::to_wstring(result.end.x));
		Log::Information(L"End y: " + std::to_wstring(result.end.y));
		Log::Information(L"GH: " + std::to_wstring(result.G_H));
		Log::Information(L"GM: " + std::to_wstring(result.G_M));
		Log::Information(L"hSideOffset: " + std::to_wstring(result.hSideOffset));
		Log::Information(L"isStraight: " + std::to_wstring(result.isStraight));
		Log::Information(L"isWeakCollision: " + std::to_wstring(result.isWeakCollision));
		Log::Information(L"P Offset x: " + std::to_wstring(result.primaryOffset.x));
		Log::Information(L"P Offset y: " + std::to_wstring(result.primaryOffset.y));
		Log::Information(L"S1 Offset x: " + std::to_wstring(result.secondary1.x));
		Log::Information(L"S1 Offset y: " + std::to_wstring(result.secondary1.y));
		Log::Information(L"S2 Offset x: " + std::to_wstring(result.secondary2.x));
		Log::Information(L"S2 Offset y: " + std::to_wstring(result.secondary2.y));
		Log::Information(L"Target x: " + std::to_wstring(result.target.x));
		Log::Information(L"Target y: " + std::to_wstring(result.target.y));
		Log::Information(L"vSideOffset: " + std::to_wstring(result.vSideOffset));
		*/
		return result;
	}

	bool CollisionEngine::FindCollision(
		const CollisionContext* const collisionContext, 
		const sf::Vector2f tileSize, 
		const sf::Vector2f tilePosition)
	{
		// quick check if precise collision
		// detection is necessary
		if (tilePosition.x + tileSize.x < collisionContext->end.x ||
			tilePosition.y + tileSize.y < collisionContext->end.y ||

			tilePosition.x > collisionContext->begin.x + currentPlayerSize.x ||
			tilePosition.y > collisionContext->begin.y + currentPlayerSize.y)
		{
			return false;
		}

		if (collisionContext->isWeakCollision && !collisionContext->isStraight)
		{
			return FindCollisionInPath(
				collisionContext,
				tileSize,
				tilePosition - collisionContext->primaryOffset,
				false);
		}
		else
		{
			return FindCollisionInPath(
					collisionContext,
					tileSize,
					tilePosition - collisionContext->primaryOffset,
					false)
				|| 
				FindCollisionInPath(
					collisionContext,
					tileSize,
					tilePosition - collisionContext->secondary1,
					true)
				||
				FindCollisionInPath(
					collisionContext,
					tileSize,
					tilePosition - collisionContext->secondary2,
					true);

			/*
			Log::Information(L"---------");
			if (FindCollisionInPath(
					collisionContext,
					tileSize,
					tilePosition - collisionContext->primaryOffset,
					false))
			{
				Log::Information(L"x: " + std::to_wstring(collisionContext->primaryOffset.x));
				Log::Information(L"y: " + std::to_wstring(collisionContext->primaryOffset.y));

				return true;
			}

			if (FindCollisionInPath(
					collisionContext,
					tileSize,
					tilePosition - collisionContext->secondary1,
					true))
			{
				Log::Information(L"x: " + std::to_wstring(collisionContext->secondary1.x));
				Log::Information(L"y: " + std::to_wstring(collisionContext->secondary1.y));

				return true;
			}

			if (FindCollisionInPath(
					collisionContext,
					tileSize,
					tilePosition - collisionContext->secondary2,
					true))
			{
				Log::Information(L"x: " + std::to_wstring(collisionContext->secondary2.x));
				Log::Information(L"y: " + std::to_wstring(collisionContext->secondary2.y));

				return true;
			}

			return false;*/
		}
	}

	CollisionEngine::CollisionInfo CollisionEngine::GetLastCollision()
	{
		return lastCollision;
	}
}
