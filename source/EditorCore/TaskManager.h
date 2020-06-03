#pragma once

#include "EditorDatabase.h"

#include "DatabaseCore/Transaction.h"

#include <cassert>
#include <deque>
#include <vector>

namespace Editor
{
	struct Task
	{
		virtual ~Task()
		{
		}

		virtual bool execute()
		{
			Database::Transaction transaction;

			if (transaction.begin(EditorDatabase::Instance()))
			{
				return false;
			}

			if (setup() && transaction.commit())
			{
				notify();
				return true;
			}

			return false;
		}

		virtual bool setup() = 0;
		virtual void notify() = 0;

		virtual void undo() = 0;
		virtual void redo() = 0;
	};

	class TaskManager
	{
		static TaskManager* instance;

		const int TaskLimit = 100;

	public:
		static TaskManager* Instance()
		{
			assert(instance != NULL);
			return instance;
		}

		TaskManager()
		{
			assert(instance == NULL);
			instance = this;
		}

		~TaskManager()
		{
			assert(instance != NULL);
			instance = NULL;
		}

		template<typename T, typename... Arg>
		bool pushTask(Arg&&... args)
		{
			return pushTask(new T(std::forward(args)...));
		}

		bool pushTask(Task* task)
		{
			if (!task->execute())
			{
				delete task;
				return false;
			}

			if (undoneTasks.size() > 0)
			{
				for (Task* task : undoneTasks)
					delete task;

				undoneTasks.clear();
			}

			if (tasks.size() > TaskLimit)
			{
				delete tasks.front();
				tasks.pop_front();
			}

			tasks.push_back(task);

			return true;
		}

		bool redo()
		{
			if (undoneTasks.empty())
				return false;

			Task* task = undoneTasks.back();
			undoneTasks.pop_back();

			task->redo();
			tasks.push_back(task);

			return true;
		}

		bool undo()
		{
			if (tasks.empty())
				return false;
			
			Task* task = tasks.back();
			tasks.pop_back();

			task->undo();
			undoneTasks.push_back(task);

			return true;
		}

	private:
		std::deque<Task*> tasks;
		std::vector<Task*> undoneTasks;
	};
}
