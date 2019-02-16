#pragma once

#include <Client/source/game/GameTileBase.h>
#include <Client/source/game/InitializableTile.h>

namespace Game
{
	struct CollisionType
	{
		const bool isWeak;

		const bool invertPlayer;
		const bool invertTile;
	private:
		char padding = 0;
	};

	class CollidableTile
		:
		public InitializableTile
	{
	public:
		CollidableTile()
		{
		}
	};
}

static bool operator<(
	const Game::CollisionType& ct1, 
	const Game::CollisionType& ct2)
{
	return *(int*) &ct1 < *(int*) &ct2;
}

static bool operator>(
	const Game::CollisionType& ct1,
	const Game::CollisionType& ct2)
{
	return *(int*) &ct1 > *(int*) &ct2
}
