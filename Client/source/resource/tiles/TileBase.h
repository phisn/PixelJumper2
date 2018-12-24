#pragma once

#include <Client/source/game/tiles/TileId.h>
#include <Client/source/resource/ResourceBase.h>

#include <SFML/Main.hpp>

namespace Resource
{
	class TileBase
		:
		public ResourceBase
	{
	public:
		TileBase(
			const Game::Tile::Id id,
			const sf::Uint64 size)
			:
			id(id),
			size(size)
		{
		}

		Game::Tile::Id getTileId() const
		{
			return id;
		}

		sf::Uint64 getSize() const
		{
			return size;
		}

	private:
		const Game::Tile::Id id;
		const sf::Uint64 size;
	};
}
