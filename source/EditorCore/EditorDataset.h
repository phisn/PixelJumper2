#pragma once

#include "Common/Notifier.h"
#include "Common/Property.h"
#include "ResourceCore/WorldResource.h"

#include <SFML/Graphics.hpp>

namespace Editor
{
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
		:
		public Dataset
	{
		ClassicWorldDataset()
			:
			Dataset()
		{
		}

		Resource::WorldID worldID;
		std::string name;

		std::vector<ClassicWorldDataset*> transitive;
		WorldTileListDataset listDataset;
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
