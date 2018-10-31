#pragma once

#include <Client/source/editor/tile/TileBase.h>

#include <SFML/Graphics.hpp>

namespace Editor
{
	class TileTemplate
	{
	public:
		const struct Settings
		{
			int maxCount;

		} settings;

		TileTemplate(
			const sf::Color color,
			const Settings settings)
			:
			color(color),
			settings(settings)
		{
		}

		sf::Color getColor() const
		{
			return color;
		}

		virtual TileBase* create(
			sf::Vector2f position,
			sf::Vector2f size) = 0;
	protected:
		sf::Color color;
	};
}