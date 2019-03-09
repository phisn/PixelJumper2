#include "TileTemplateFactory.h"

#include <Client/source/editor/template/WallTemplate.h>

#include <map>

namespace
{
	std::map<Game::TileId, Editor::TileTemplate*> tileTemplates;
}

#define INSERT_TILETEMPLATE_DIFFER(gname, ename) \
	tileTemplates[Game::TileId::##gname] = new ename##Template()
#define INSERT_TILETEMPLATE(name) INSERT_TILETEMPLATE_DIFFER(name, name)

namespace Editor
{
	void TileTemplateFactory::Initialize()
	{
		INSERT_TILETEMPLATE(Wall);
	}

	void TileTemplateFactory::Uninitialize()
	{
		for (const std::pair<Game::TileId, Editor::TileTemplate*>& tt : tileTemplates)
		{
			delete tt.second;
		}

		tileTemplates.clear();
	}

	TileTemplate* TileTemplateFactory::GetTileTemplate(const Game::TileId id)
	{
		return tileTemplates[id];
	}
}
