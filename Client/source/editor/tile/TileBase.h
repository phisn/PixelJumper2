#pragma once

#include <SFML/Graphics.hpp>

#include 

#include <Client/source/game/tiles/TileId.h>

namespace Editor
{
	class TileBase
	{
	public:
		TileBase(
			Game::Tile::Id id)
			:
			id(id)
		{
		}

		Game::Tile::Id getId() const
		{
			return id;
		}

		sf::Vector2f position;
		sf::Vector2f size;

		virtual Game::Tile::Base* create() = 0;
	private:
		Game::Tile::Id id;
	};
}