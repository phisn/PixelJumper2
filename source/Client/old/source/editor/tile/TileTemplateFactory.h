#pragma once

#include <Client/source/editor/tile/TileTemplate.h>

#include <Client/source/shared/tiles/TileCommon.h>

namespace Editor
{
	namespace TileTemplateFactory
	{
		void Initialize();
		void Uninitialize();

		TileTemplate* GetTileTemplate(const Shared::TileID id);
	}
}
