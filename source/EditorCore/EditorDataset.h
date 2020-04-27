#pragma once

#include "Common/Notifier.h"

#include <SFML/Graphics.hpp>

namespace Editor
{
	class Dataset
		:
		public Util::Notifier<Dataset>
	{
	public:
		Dataset(Dataset* parent = NULL)
			:
			parent(parent)
		{
		}

		void notify()
		{
			Notifier::notify();

			if (parent)
			{
				parent->notify();
			}
		}

	private:
		Dataset* parent;
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

	struct ClassicWorldDataset
		:
		public Dataset
	{
		std::string name;

		std::vector<WorldTileDataset> tiles;
	};
	
	struct ClassicStageDataset
		:
		public Dataset
	{
	};

	struct ClassicContextDataset
		:
		public Dataset
	{
	public:
		std::vector<ClassicWorldDataset*> worlds;
	};
}
