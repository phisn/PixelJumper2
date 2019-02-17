#pragma once

namespace Game
{
	class TileContainer;

	struct CollisionType
	{
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

	class CollidableTile
	{
	protected:
		void addCollisionType(TileContainer* const container, const CollisionType type);
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
