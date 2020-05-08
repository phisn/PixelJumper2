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
	struct Task
		:
		public AbstractTask
	{
		virtual void execute(Dataset* dataset) = 0;
	};

	class _AbstractDataset
	{
	public:
		virtual void undo() = 0;
		virtual void redo() = 0;
	};

	class _Dataset
		:
		public _AbstractDataset
	{
	public:
		void pushTask(AbstractTask* task)
		{
			undoTasks.push_back(task);

			if (undoTasks.size() > taskLimit)
				undoTasks.pop_front();
		}

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
		_Dataset* root;
	};

	class Dataset
		:
		public Util::Notifier<Dataset>
	{
	public:
		using Notifier::notify;
	};

	class LazyDatasetChange
	{
	public:
		LazyDatasetChange(Dataset* dataset)
			:
			dataset(dataset)
		{
		}

		~LazyDatasetChange()
		{
			if (dataChanged)
				dataset->notify();
		}

		void notify()
		{
			dataChanged = true;
		}

		bool hasDataChanged() const
		{
			return dataChanged;
		}

	private:
		bool dataChanged = false;
		Dataset* dataset;
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

	struct ClassicWorldDataset
	{
		Resource::WorldID worldID;
		std::string name;

		std::vector<ClassicWorldDataset*> transitive;
		WorldTileListDataset listDataset;
	};

	class ClassicWorldDatasetContainer
		:
		public _DatasetChild
	{
	public:
		const ClassicWorldDataset* operator->() const
		{
			return &dataset;
		}

		const ClassicWorldDataset* getDataset() const
		{
			return &dataset;
		}

	private:
		ClassicWorldDataset dataset;
	};
	
	struct ClassicStageDataset
		:
		public _Dataset
	{
	};

	struct ClassicContextDataset
		:
		public _Dataset
	{
	public:
		std::vector<ClassicWorldDataset*> worlds;
	};
}
