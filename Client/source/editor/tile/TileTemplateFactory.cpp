#include "TileTemplateFactory.h"

#include <Client/source/shared/tiles/TileDescription.h>

#include <map>

namespace
{
	std::map<Shared::TileID, Editor::TileTemplate*> tileTemplates;
}

namespace Editor
{
	void TileTemplateFactory::Initialize()
	{
		for (int i = 1; i < (int) Shared::TileID::_Length; ++i)
		{
			tileTemplates[(Shared::TileID) i] = Shared::TileDescription::Find(
				(Shared::TileID) i
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

	TileTemplate* TileTemplateFactory::GetTileTemplate(const Shared::TileID id)
	{
		return tileTemplates[id];

	}
}
