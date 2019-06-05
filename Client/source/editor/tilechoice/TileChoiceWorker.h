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
			output->selection = input->selection;
			return true;
		}
	};
}
