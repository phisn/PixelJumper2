#pragma once

#include <Client/source/game/CollisionEngine.h>
#include <Client/source/game/PlayerState.h>

namespace Game
{
	class TileContainer;

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

	struct Collision
	{
		CollisionEngine::CollisionInfo info;

		PlayerState* player;
		sf::Vector2f target; // expected destination
	};

	class CollidableTile
	{
	public:
		/*
			Return: repeated movement without physics,
			used for sliding or bouncing (normal walls slide too [ignores phyiscs])
		*/
		virtual sf::Vector2f onCollision(
			const CollisionType type,
			const Collision collision) = 0;

		// copied from / same as in GameTileBase.h
		virtual const sf::Vector2f getPosition() const = 0;
		virtual const sf::Vector2f getSize() const = 0;
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

static bool operator==(
	const Game::CollisionType& ct1,
	const Game::CollisionType& ct2)
{
	return *(int*)&ct1 == *(int*)&ct2
}
