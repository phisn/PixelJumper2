#pragma once

#include <Client/source/editor/manipulator/cache/Cache.h>
#include <Client/source/editor/manipulator/Executor.h>
#include <Client/source/editor/manipulator/Manipulator.h>

namespace Editor
{
	class TilePlace
		:
		public Task
	{
	public:
		bool execute() override
		{
			const Cache::Output* const cache = Manipulator::getCache()->readOutput();

			if (cache->selection.type == SelectionCache::Output::Type::Area)
			{

			}
			else
			{

			}
		}

		void redo() override
		{

		}

		void undo() override
		{

		}

	private:
		struct
		{


		} newContent;

		struct
		{


		} oldContent;
	}
}
