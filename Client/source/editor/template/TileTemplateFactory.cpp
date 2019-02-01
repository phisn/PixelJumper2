#include "TileTemplateFactory.h"

#include <Client/source/editor/template/WallTemplate.h>

#include <map>

namespace
{
	std::map<Game::Tile::Id, Editor::TileTemplate*> tileTemplates;
}

#define INSERT_TILETEMPLATE_DIFFER(gname, ename) \
	tileTemplates[Game::Tile::Id::##gname] = new ename##Template()
#define INSERT_TILETEMPLATE(name) INSERT_TILETEMPLATE_DIFFER(name, name)

namespace Editor
{
	void TileTemplateFactory::Initialize()
	{
		INSERT_TILETEMPLATE(Wall);
	}

	void TileTemplateFactory::Uninitialize()
	{
		for (const std::pair<Game::Tile::Id, Editor::TileTemplate*>& tt : tileTemplates)
		{
			delete tt.second;
		}

		tileTemplates.clear();
	}

	TileTemplate* TileTemplateFactory::GetTileTemplate(
		const Game::Tile::Id id)
	{
		return tileTemplates[id];
	}
}
