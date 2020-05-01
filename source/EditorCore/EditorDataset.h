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
		Dataset()
			:
			Dataset(NULL)
		{
		}

		Dataset(Dataset* parent)
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
		ClassicWorldDataset()
			:
			Dataset()
		{
			auto changeListener = [this]()
			{
				notify();
			};

			worldID.addListener(changeListener);
			name.addListener(changeListener);
			transitive.addListener(changeListener);
			tiles.addListener(changeListener);
		}

		Util::Property<Resource::WorldID> worldID;
		Util::Property<std::string> name;

		Util::Property<std::vector<ClassicWorldDataset*>> transitive;
		Util::Property<std::vector<WorldTileDataset>> tiles;
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
