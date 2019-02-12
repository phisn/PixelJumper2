#pragma once

#include <SFML/Graphics.hpp>

namespace Game
{
	struct StaticTileProperties
	{
		bool isEnterable;
		bool isLeaveable;
		bool isStatic;
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

		virtual void onDraw() = 0;
		virtual void onLogic(const sf::Time) = 0;

		virtual const sf::Vector2f getPosition() const = 0;
		virtual const sf::Vector2f getSize() const = 0;

		const sf::Color getColor() const
		{
			return color;
		} // non movable or changeable tiles

		const StaticTileProperties* getTileProperties() const
		{
			return stp;
		}

	private:
		const StaticTileProperties* const stp;

		const sf::Color color;
	};
}
