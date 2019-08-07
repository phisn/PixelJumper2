#pragma once

#include <Client/source/editor/tile/TileTemplate.h>
#include <Client/source/game/tile/TileId.h>

namespace Editor
{
	namespace TileTemplateFactory
	{
		void Initialize();
		void Uninitialize();

		TileTemplate* GetTileTemplate(const Game::TileId id);
	}
}
