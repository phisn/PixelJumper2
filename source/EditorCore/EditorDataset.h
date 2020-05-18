#pragma once

#include "EditorDatabase.h"

#include "ResourceCore/ClassicContextResource.h"
#include "ResourceCore/component/DynamicTransitionComponent.h"

namespace Editor
{
	struct Task
	{
		virtual ~Task()
		{
		}

		virtual void undo() = 0;
		virtual void redo() = 0;
	};

	struct ContextDataset
	{
		ContextDataset(ContextTuple tuple)
			:
			contextID(std::get<ContextColumn::ID>(tuple)),
			name(std::get<ContextColumn::Name>(tuple)),
			description(std::get<ContextColumn::Description>(tuple))
		{
		}

		Resource::ContextID contextID;
		std::string name;
		std::string description;
	};

	struct ContextEvent
	{
		Resource::ContextID contextID;
	};

	struct WorldEvent
	{
		Resource::WorldID worldID;
		Resource::ContextID contextID;
	};

	struct TransitiveEvent
	{
		Resource::WorldEntryID entryID;
		Resource::WorldID outputWorldID;
		Resource::WorldID inputWorldID;
		Resource::ContextID contextID;
	};

	struct TileEvent
	{
		Resource::WorldID worldID;
	};
}
/*
#include "Common/Notifier.h"
#include "Common/Property.h"

#include "SFML/Graphics.hpp"

#include <deque>


namespace Editor
{
	typedef uint64_t DatasetID;

	struct AbstractTask
	{
		virtual ~AbstractTask()
		{
		}

		virtual void undo() = 0;
		virtual void redo() = 0;
	};

	template <typename Dataset>
	class Task
		:
		public AbstractTask
	{
	public:
		bool execute(Dataset* dataset)
		{
			this->dataset = dataset;
			return execute();
		}

	protected:
		virtual bool execute()
		{
			redo();
		}

		Dataset* dataset;
	};

	enum class DatasetEvent
	{
		Changed,
		Removed
	};

	enum class DatasetType
	{
		Context,
		World,
		Transitive,
		WorldTileContainer,
		WorldTile,
	};

	class AbstractDataset
		:
		public Util::Notifier<AbstractDataset, DatasetEvent>
	{
		friend class DatasetManagment;

		AbstractDataset(AbstractDataset&) = delete;
		AbstractDataset& operator=(AbstractDataset&) = delete;

	public:
		using Notifier::Notifier;
		using Notifier::notify;

		virtual ~AbstractDataset()
		{
		}

		virtual void undo() = 0;
		virtual void redo() = 0;

		virtual DatasetType getType() const = 0;

		DatasetID getDatasetID() const
		{
			return datasetID;
		}

	protected:
		virtual bool saveDynamic(Resource::WritePipe* pipe) = 0;
		virtual bool saveStatic(Resource::WritePipe* pipe) = 0;

		virtual bool loadDynamic(Resource::ReadPipe* pipe) = 0;
		virtual bool loadStatic(Resource::ReadPipe* pipe) = 0;

	private:
		// managed by datasetmanagment
		DatasetID datasetID;
	};

	class Dataset
		:
		public AbstractDataset
	{
		friend class DatasetChild;

	public:
		void undo() override
		{
			if (undoTasks.size() > 0)
			{
				undoTasks.back()->undo();
				redoTasks.push_back(undoTasks.back());
				undoTasks.pop_back();
			}
		}

		void redo() override
		{
			if (redoTasks.size() > 0)
			{
				redoTasks.back()->redo();
				undoTasks.push_back(redoTasks.back());
				redoTasks.pop_back();
			}
		}

	protected:
		int taskLimit = 50;

		void pushTask(AbstractTask* task)
		{
			undoTasks.push_back(task);

			if (undoTasks.size() > taskLimit)
				undoTasks.pop_front();
		}

	private:
		std::deque<AbstractTask*> undoTasks;
		std::deque<AbstractTask*> redoTasks;
	};

	class DatasetChild
		:
		public AbstractDataset
	{
	public:
		DatasetChild(Dataset* root)
			:
			root(root)
		{
		}

		void undo() override
		{
			root->undo();
		}

		void redo() override
		{
			root->redo();
		}

	private:
		void pushTask(AbstractTask* task)
		{
			root->pushTask(task);
		}

	protected:
		Dataset* root;
	};

	template <typename DatasetContent, typename DatasetType = Dataset>
	class CommonDataset
		:
		public DatasetType
	{
	public:
		template <typename... Args>
		CommonDataset(Args&&... args)
			:
			DatasetType(),
			dataset(std::forward<Args>(args)...)
		{
		}

		CommonDataset()
			:
			DatasetType()
		{
		}

		bool execute(Task<DatasetContent>* task)
		{
			if (task->execute(&dataset))
			{
				pushTask(task);
				notify(DatasetEvent::Changed);

				return true;
			}

			return false;
		}

		const DatasetContent* operator->() const
		{
			return &dataset;
		}

		const DatasetContent& getDataset() const
		{
			return dataset;
		}

		const DatasetContent& operator*() const
		{
			return dataset;
		}

	protected:
		DatasetContent dataset;
	};
}
*/