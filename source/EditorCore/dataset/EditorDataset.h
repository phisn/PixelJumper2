#pragma once

#include "Common/Notifier.h"
#include "Common/Property.h"
#include "ResourceCore/WorldResource.h"

#include <SFML/Graphics.hpp>

namespace Editor
{
	struct AbstractTask
	{
		virtual void undo() = 0;
		virtual void redo() = 0;
	};

	template <typename Dataset>
	class Task
		:
		public AbstractTask
	{
	public:
		virtual bool execute(Dataset* dataset) = 0
		{
			this->dataset = dataset;
		}

	protected:
		Dataset* dataset;
	};

	class _AbstractDataset
		:
		public Util::Notifier<_AbstractDataset>
	{
		_AbstractDataset(_AbstractDataset&) = delete;
		_AbstractDataset& operator=(_AbstractDataset&) = delete;

	public:
		using Notifier::Notifier;

		virtual void undo() = 0;
		virtual void redo() = 0;

	protected:
		using Notifier::notify;
	};

	class _Dataset
		:
		public _AbstractDataset
	{
		friend class _DatasetChild;

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

	class _DatasetChild
		:
		public _AbstractDataset
	{
	public:
		_DatasetChild(_Dataset* root)
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
		_Dataset* root;
	};

	template <
		typename DatasetType,
		typename DatasetContent>
		class _CommonDataset
		:
		public DatasetType
	{
	public:
		using DatasetType::DatasetType;

		bool execute(Task<DatasetContent>* task)
		{
			if (task->execute(&dataset))
			{
				pushTask(task);
				notify();

				return true;
			}

			return false;
		}

		const DatasetContent* operator->() const
		{
			return &dataset;
		}

		const DatasetContent* getDataset() const
		{
			return &dataset;
		}

	protected:
		DatasetContent dataset;
	};

	struct WorldTileDataset
		:
		public Dataset
	{
		sf::Vector2i position;
		sf::Vector2i size;
	};

	struct WorldTileListDataset
		:
		public Dataset
	{
		std::vector<WorldTileDataset> tiles;
	};

	struct ClassicWorldDatasetContent
	{
		Resource::WorldID worldID;
		std::string name;

		std::vector<ClassicWorldDataset*> transitive;
		WorldTileListDataset listDataset;
	};

	typedef _CommonDataset<_DatasetChild, ClassicWorldDatasetContent> ClassicWorldDataset;

	struct ClassicStageDataset
		:
		public _Dataset
	{
	};

	struct ClassicContextDatasetContent
	{
		std::vector<ClassicWorldDataset*> worlds;
	};

	typedef _CommonDataset<_Dataset, ClassicWorldDatasetContent> ClassicContextDataset;
}
