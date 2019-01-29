#pragma once

#include <Client/source/editor/manipulator/cache/Cache.h>
#include <Client/source/editor/EditorWorld.h>

#include <deque>
#include <string>
#include <vector>

#define TASK_LIMIT 1024

namespace Editor
{
	class Task
	{
	public:
		_Success_(return == true) 
		virtual bool execute(
			World* const world) = 0;

		virtual void redo() = 0;
		virtual void undo() = 0;

		const std::wstring* getName() const
		{
			return &name;
		}

		const std::wstring* getDescription() const
		{
			return &description;
		}

	protected:
		std::wstring name;
		std::wstring description;
	};

	class Executor
	{
	public:
		Executor(World* const world)
			:
			world(world)
		{
		}

		~Executor()
		{
			for (Task* const task : tasks)
			{
				delete task;
			}

			for (Task* const task : redoTasks)
			{
				delete task;
			}
		}

		template <typename _Task>
		void execute()
		{
			execute(new _Task());
		}

		void execute(Task* task)
		{
			if ( task->execute(world) )
			{
				tasks.push_front(task);

				if (tasks.size() > TASK_LIMIT)
				{
					tasks.pop_back();
				}

				if (redoTasks.empty())
				{
					redoTasks.clear(); 
				}
			}
		}

		void undo(int count = 1)
		{
			for (int i = 0; i < count; ++i)
			{
				if (tasks.empty())
				{
					return;
				}

				Task* const task = tasks.front();

				redoTasks.push_back(task);
				task->undo();

				tasks.pop_front();
			}
		}

		void redo(int count = 1)
		{
			for (int i = 0; i < count; ++i)
			{
				if (redoTasks.empty())
				{
					return;
				}

				Task* const task = redoTasks.back();

				tasks.push_back(task);
				task->redo();

				redoTasks.pop_back();
			}
		}

		const std::deque<Task*>* getUndoTasks() const
		{
			return &tasks;
		}

		const std::vector<Task*>* getRedoTasks() const
		{
			return &redoTasks;
		}

	private:
		std::deque<Task*> tasks;
		std::vector<Task*> redoTasks;

		World* const world;
	};
}
