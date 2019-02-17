#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include <Client/source/game/tiletrait/GameTileBase.h>

namespace Game
{
	class StaticTile
		:
		public GameTileBase
	{
	public:
		StaticTile(
			const sf::Color color,
			const sf::Vector2f position,
			const sf::Vector2f size)
			:
			color(color),
			position(position),
			size(size)
		{
		}

		sf::Color getColor() const
		{
			return color;
		}

		const sf::Vector2f getPosition() const
		{
			return position;
		}

		const sf::Vector2f getSize() const
		{
			return size;
		}

		virtual void initialize(TileContainer* const container) override;
	protected:
		const sf::Color color;

		const sf::Vector2f position;
		const sf::Vector2f size;
	};
}