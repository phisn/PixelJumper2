#pragma once

#include <Client/source/editor/template/TileTemplate.h>
#include <Client/source/game/tiles/TileId.h>

namespace Editor
{
	namespace TileTemplateFactory
	{
		void Initialize();
		void Uninitialize();

		TileTemplate* GetTileTemplate(
			const Game::Tile::Id id);
	}
}
