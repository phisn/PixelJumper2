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
		Wall()
			:
			TileBase(Game::Tile::Id::Wall)
		{
		}

		Game::Tile::Base* create() override
		{
			return new Game::Tile::Wall(
				{
					position,
					size
				});
		}
	};
}