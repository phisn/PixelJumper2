#pragma once
#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Manipulator.h>

#include <Client/source/editor/tile/TileTemplateFactory.h>

namespace Editor
{
	class TileChoiceWorker
	{
	public:
		bool operator()(
			const TileChoiceInput* const input,
			TileChoiceOutput* const output)
		{
			if (output->selection != NULL && output->selection->isSelected())
			{
				output->selection->unselect();
			}

			if (!input->selection->isSelected())
			{
				input->selection->select();
			}

			output->selection = input->selection;
			return true;
		}
	};
}
