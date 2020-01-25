#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include <Client/source/game/tiletrait/GameTileBase.h>
#include <Client/source/game/Environment.h>

namespace Game
{
	class StaticTile
		:
		public virtual GameTileBase
	{
	public:
		StaticTile(const sf::Color color)
			:
			color(color)
		{
		}

		sf::Color getColor() const
		{
			return color;
		}

	protected:
		void registerType(Environment* const env)
		{
			env->registerTile<StaticTile>(this);
		}

		const sf::Color color;
	};
}