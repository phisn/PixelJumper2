#pragma once

#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Manipulator.h>

#include <Client/source/editor/template/TileTemplateFactory.h>

namespace Editor
{
	class TileChoiceWorker
	{
	public:
		bool operator()(
			const TileChoiceInput* const input,
			TileChoiceOutput* const output)
		{
			this->input = input;
			this->output = output;

			output->tiles.push_back(
				TileTemplateFactory::GetTileTemplate(Game::TileId::Wall)
			);

			return true;
		}
		
	private:
		const TileChoiceInput* input;
		TileChoiceOutput* output;
	};
}
