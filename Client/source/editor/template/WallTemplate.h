#pragma once

#include <Client/source/game/tile/GameWallTile.h>

#include <Client/source/editor/template/TileTemplate.h>
#include <Client/source/editor/template/TileTemplateFactory.h>
#include <Client/source/editor/tile/EditorWall.h>

#include <iostream>

namespace Editor
{
	class WallTemplate
		:
		public TileTemplate
	{
	public:
		WallTemplate()
			:
			TileTemplate(
				Game::WallTile::COLOR,
				L"Wall",
				Game::TileId::Wall)
		{
		}

		// onOpenContext ...

		// ex. can be saved and shon in context menu (portal binding)
		TileBase* create(
			VectorTilePosition position) override
		{
			return new Tile::Wall(position);
		}
	};
}
