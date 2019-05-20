#pragma once

#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Manipulator.h>

#include <Client/source/editor/selector/SelectorCache.h>

#include <SFML/Graphics.hpp>

namespace Editor
{
	class SelectorConverter
	{
	public:
		bool operator()(
			const SelectorInput* const input,
			SelectorOutput* const output)
		{
			this->input = input;
			this->output = output;

			if (input->type != oldSelectorType)
			{
				updateNewSelectionType();
			}

			// ...
		}

	private:
		const SelectorInput* input;
		SelectorOutput* output;

		void updateNewSelectionType()
		{
			switch (this->input.)
		}

		SelectorType oldSelectorType = (SelectorType) -1;
	};

	class SelectorMarker
	{
	public:
		SelectorMarker()
		{
			Manipulator::GetCache()->selection.addListener(*this);
			Manipulator::GetCache()->selection.addWorker(
				converter,
				DefaultConverterPriority
			);
		}

		void operator()(const SelectorOutput* const output)
		{
		}

	private:
		sf::RectangleShape outlineMarker, tileMarker;

		SelectorConverter converter;
	};
}