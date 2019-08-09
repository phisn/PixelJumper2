#include "TileTemplateFactory.h"

#include <Client/source/shared/tiles/TileDescription.h>

#include <map>

namespace
{
	std::map<Shared::TileId, Editor::TileTemplate*> tileTemplates;
}

namespace Editor
{
	void TileTemplateFactory::Initialize()
	{
		for (int i = 0; i < (int) Shared::TileId::_Length; ++i)
		{
			tileTemplates[(Shared::TileId) i] = Shared::TileDescription::Find(
				(Shared::TileId) i
			)->creation.createEditorTemplate();
		}
	}

	void TileTemplateFactory::Uninitialize()
	{
		for (decltype(tileTemplates)::const_reference tt : tileTemplates)
		{
			delete tt.second;
		}

		tileTemplates.clear();
	}

	TileTemplate* TileTemplateFactory::GetTileTemplate(const Shared::TileId id)
	{
		return tileTemplates[id];
	}
}
