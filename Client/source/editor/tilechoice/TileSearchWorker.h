#pragma once

#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Manipulator.h>

#include <Client/source/editor/tile/TileTemplateFactory.h>

namespace Editor
{
	class TileSearchWorker
	{
	public:
		bool operator()(
			const TileSearchInput* const input,
			TileSearchOutput* const output)
		{
			this->input = input;
			this->output = output;

			output->tiles.push_back(
				TileTemplateFactory::GetTileTemplate(Shared::TileId::TileWall)
			);

			return true;
		}
		
	private:
		const TileSearchInput* input;
		TileSearchOutput* output;
	};
}
