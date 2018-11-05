#pragma once

#include <Client/source/game/tiles/TileId.h>

#include <Client/source/editor/template/TileTemplate.h>

namespace Editor
{
	namespace TemplateManager
	{
		// add possibility to iterate (TileMenu) -> Templates as base

		TileTemplate* getTemplate(
			const Game::Tile::Id tileId);
		void registerTemplate(
			Game::Tile::Id id,
			TileTemplate* templ);
		void clearTemplates();
	}
}
