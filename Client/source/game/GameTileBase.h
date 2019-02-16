#pragma once

#include <SFML/Graphics.hpp>

namespace Game
{
	struct StaticTileProperties
	{
		bool isCollidable;
		bool isInitializable;
		bool isStatic;
		bool isDynamic;
	};

	class GameTileBase
	{
	public:
		GameTileBase(
			const StaticTileProperties* const stp)
			:
			stp(stp)
		{
		}

		virtual const sf::Vector2f getPosition() const = 0;
		virtual const sf::Vector2f getSize() const = 0;

		const StaticTileProperties* getTileProperties() const
		{
			return stp;
		}

	private:
		const StaticTileProperties* const stp;
	};
}

Game::StaticTileProperties operator|=(
	const Game::StaticTileProperties& stp1, 
	const Game::StaticTileProperties& stp2)
{
	for (int i = 0; i < sizeof(Game::StaticTileProperties); ++i)
	{
		((char*) &stp1)[i] |= ((char*) &stp2) [i];
	}
}
