#pragma once

#include <Client/source/game/tiles/TileColor.h>

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
				TILE_WALL_COLOR,
				L"Wall",
				Game::Tile::Id::Wall)
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
