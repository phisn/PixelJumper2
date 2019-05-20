#pragma once

#include <Client/source/editor/manipulator/EditorTask.h>

#include <deque>
#include <string>
#include <vector>

#define TASK_LIMIT 32

namespace Editor
{
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
					delete tasks.back();
					tasks.pop_back();
				}

				if (!redoTasks.empty())
				{
					for (Task* const task : redoTasks)
					{
						delete task;
					}

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
				task->undo(world);

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

				tasks.push_front(task);
				task->redo(world);

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
