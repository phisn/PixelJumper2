#pragma once

#include <SFML/Graphics.hpp>

#include <deque>

namespace Editor
{
	class Task
	{
	public:
		virtual void undo() = 0;
		virtual void redo() = 0;
	};

	class EditorWindow
	{
	public:
		EditorWindow()
		{
		}

		virtual void onProcess() = 0;
		virtual void onEvent(sf::Event event) = 0;

	protected:
		int taskLimit = 100;

		void pushTask(Task* task)
		{
			undoTasks.push_back(task);

			if (undoTasks.size() > taskLimit)
				undoTasks.pop_front();
		}

		void undo()
		{
			if (undoTasks.size() > 0)
			{
				undoTasks.back()->undo();
				redoTasks.push_back(undoTasks.back());
				undoTasks.pop_back();
			}
		}

		void redo()
		{
			if (redoTasks.size() > 0)
			{
				redoTasks.back()->redo();
				undoTasks.push_back(redoTasks.back());
				redoTasks.pop_back();
			}
		}
		
	private:
		std::deque<Task*> undoTasks;
		std::vector<Task*> redoTasks;
	};
}
