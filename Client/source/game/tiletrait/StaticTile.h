#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include <Client/source/game/tiletrait/GameTileBase.h>
#include <Client/source/game/Environment.h>

namespace Game
{
	class StaticTile
		:
		public GameTileBase
	{
	public:
		virtual void registerType(Environment* const env) override
		{
			env->registerTile<StaticTile>(this);
		}

		StaticTile(
			const int identity,
			const sf::Color color,
			const sf::Vector2f position,
			const sf::Vector2f size)
			:
			GameTileBase(identity),
			color(color),
			position(position),
			size(size)
		{
		}

		sf::Color getColor() const
		{
			return color;
		}

		const sf::Vector2f getPosition() const override
		{
			return position;
		}

		const sf::Vector2f getSize() const override
		{
			return size;
		}

	protected:
		const sf::Color color;

		const sf::Vector2f position;
		const sf::Vector2f size;
	};
}