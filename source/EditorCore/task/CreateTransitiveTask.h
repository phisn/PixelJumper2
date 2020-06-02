#pragma once

#include "TaskManager.h"
#include "EditorDatabase.h"

#include "ResourceCore/component/DynamicTransitionComponent.h"

namespace Editor
{
	class TransitiveTaskBase
		:
		public Task
	{
	protected:
		Resource::ContextID contextID;
		std::string name;

		Resource::WorldID outputID;
		Resource::WorldID inputID;

		Resource::WorldEntryID entryID;

		bool insert()
		{
			return Database::Statement(
				EditorDatabase::Instance(),
				"INSERT INTO transitive (id, name, outputid, inputid, contextid) VALUES (?, ?, ?, ?, ?)",
				entryID,
				name,
				outputID,
				inputID,
				contextID).execute();
		}

		bool remove()
		{
			return Database::Statement(
				EditorDatabase::Instance(),
				"DELETE FROM transitive WHERE id = ?",
				entryID).execute();
		}
	};

	class CreateTransitiveTask
		:
		public TransitiveTaskBase
	{
	public:
		CreateTransitiveTask(
			Resource::ContextID contextID,
			std::string name,
			Resource::WorldID outputID,
			Resource::WorldID inputID)
		{
			this->contextID = contextID;
			this->name = name;
			this->outputID = outputID;
			this->inputID = inputID;
		}

		bool setup() override
		{
			entryID = Module::Random::MakeRandom<Resource::WorldEntryID>();
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
					"Failed to undo transitive creation'" + std::to_string(entryID) + "'"));
			}

			makeDatabaseEvent(DatabaseEventType::Remove);
		}

		void redo() override
		{
			if (!insert())
			{
				Framework::Core::PushChildScene(new EditorFailureScene(
					"Failed to redo transitive creation'" + std::to_string(entryID) + "'"));
			}

			makeDatabaseEvent(DatabaseEventType::Create);
		}

	private:
		void makeDatabaseEvent(DatabaseEventType type)
		{
			DatabaseEvent event;
			
			event.type = type;
			event.table = DatabaseTable::Transitive;

			event.data.transitive.contextID = contextID;
			event.data.transitive.entryID = entryID;
			event.data.transitive.inputWorldID = inputID;
			event.data.transitive.outputWorldID = outputID;

			WindowManager::Instance()->notifyDatabaseEvent(event);
		}
	};
}
