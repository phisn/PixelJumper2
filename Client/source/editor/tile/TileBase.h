#pragma once

#include <SFML/Graphics.hpp>

#include <Client/source/game/tiles/TileBase.h>
#include <Client/source/game/tiles/TileId.h>

namespace Editor
{
	struct TileSettings
	{
		sf::Color color;
		sf::Vector2f position;
	};

	class TileBase
	{
	public:
		TileBase(
			const Game::Tile::Id id,
			const TileSettings settings)
			:
			id(id)
		{
			shape.setFillColor(settings.color);
			shape.setPosition(settings.position);
			shape.setSize(
				sf::Vector2f(1.f, 1.f)
			);
		}

		Game::Tile::Id getId() const
		{
			return id;
		}

		sf::RectangleShape* getShape()
		{
			return &shape;
		}

		virtual Game::Tile::Base* create() = 0;
	private:
		const Game::Tile::Id id;
		sf::RectangleShape shape;
	};
}