#pragma once

#include <Client/source/editor/tile/TileBase.h>

#include <Client/source/game/tiles/Wall.h>

namespace Editor
{
	class Wall
		:
		public TileBase
	{
	public:
		Wall(
			const TileSettings settings)
			:
			TileBase(
				Game::Tile::Id::Wall,
				settings)
		{
		}

		Game::Tile::Base* create() override
		{
			return new Game::Tile::Wall(
				{
					getShape()->getPosition(),
					getShape()->getSize()
				});
		}
	};
}