#pragma once

#include <Client/source/editor/manipulator/EditorWorld.h>

namespace Editor
{
	class Task
	{
	public:
		virtual bool execute(World* const world) = 0;

		virtual void redo(World* const world) = 0;
		virtual void undo(World* const world) = 0;

		const std::wstring& getName() const
		{
			return name;
		}

		const std::wstring& getDescription() const
		{
			return description;
		}

	protected:
		std::wstring name;
		std::wstring description;
	};

}
