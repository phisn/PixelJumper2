#pragma once

#include "CreateTransitiveTask.h"

namespace Editor
{
	class WorldTaskBase
		:
		public Task
	{
	public:
		Resource::WorldID getWorldID() const
		{
			return worldID;
		}

	protected:
		Resource::ContextID contextID;
		std::string name;
		Resource::WorldID worldID;

		void makeDatabaseEvent(DatabaseEventType type)
		{
			DatabaseEvent event;

			event.type = type;
			event.table = DatabaseTable::World;

			event.data.world.contextID = contextID;
			event.data.world.worldID = worldID;

			WindowManager::Instance()->notifyDatabaseEvent(event);
		}

		bool insert()
		{
			return Database::Statement(
				EditorDatabase::Instance(),
				"INSERT INTO world (id, name, contextid) VALUES (?, ?, ?)",
				worldID,
				name,
				contextID).execute();
		}

		bool remove()
		{
			return Database::Statement(
				EditorDatabase::Instance(),
				"DELETE FROM world WHERE id = ?",
				worldID).execute();
		}
	};

	class CreateWorldTask
		:
		public WorldTaskBase
	{
	public:
		CreateWorldTask(
			Resource::ContextID contextID,
			std::string name)
		{
			this->contextID = contextID;
			this->name = name;
		}

		bool setup() override
		{
			worldID = Module::Random::MakeRandom<Resource::WorldID>();
			return insert();
		}

		void notify() override
		{
			makeDatabaseEvent(DatabaseEventType::Create);
		}

		void undo() override
		{
			if (!remove())
			{
				Framework::Core::PushChildScene(new EditorFailureScene(
					"Failed to undo world creation'" + std::to_string(worldID) + "'"));

				return;
			}

			makeDatabaseEvent(DatabaseEventType::Remove);
		}

		void redo() override
		{
			if (!insert())
			{
				Framework::Core::PushChildScene(new EditorFailureScene(
					"Failed to redo world creation'" + std::to_string(worldID) + "'"));

				return;
			}

			makeDatabaseEvent(DatabaseEventType::Create);
		}
	};

	class RemoveWorldTask
		:
		public WorldTaskBase
	{
	public:
		RemoveWorldTask(Resource::WorldID id)
		{
			worldID = id;
		}

		~RemoveWorldTask()
		{
			for (Task* task : childs)
				delete task;
		}

		bool setup() override
		{
			typedef std::tuple<SQLiteInt, SQLiteString> WorldTuple;

			WorldTuple world;
			if (!Database::Statement<WorldTuple>(
					EditorDatabase::Instance(),
					"SELECT contextid, name FROM world WHERE id = ?",
					worldID).execute(world))
			{
				return false;
			}

			contextID = std::get<0>(world);
			name = std::get<1>(world);

			typedef std::tuple<SQLiteInt> TransitiveTuple;

			Database::Statement<TransitiveTuple> statement(
				EditorDatabase::Instance(),
				"SELECT id FROM transitive WHERE outputid = ? OR inputid = ?",
				worldID,
				worldID);

			for (const TransitiveTuple& transitive : statement)
			{
				RemoveTransitiveTask* task = new RemoveTransitiveTask(std::get<0>(transitive));
				childs.push_back(task);

				if (!task->setup())
					return false;
			}

			if (!statement)
			{
				return false;
			}

			return remove();
		}

		void notify() override
		{
			makeDatabaseEvent(DatabaseEventType::Remove);

			for (Task* task : childs)
				task->notify();
		}

		void undo() override
		{
			if (!insert())
			{
				Framework::Core::PushChildScene(new EditorFailureScene(
					"Failed to redo world removal'" + std::to_string(worldID) + "'"));

				return;
			}

			makeDatabaseEvent(DatabaseEventType::Create);

			for (Task* task : childs)
				task->undo();
		}

		void redo() override
		{
			for (Task* task : childs)
				task->redo();

			if (!remove())
			{
				Framework::Core::PushChildScene(new EditorFailureScene(
					"Failed to undo world removal'" + std::to_string(worldID) + "'"));

				return;
			}

			makeDatabaseEvent(DatabaseEventType::Remove);
		}

	private:
		std::vector<Task*> childs;
	};
}
