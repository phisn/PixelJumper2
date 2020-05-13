#pragma once

#include "WorldTileDataset.h"

namespace Editor
{
	class ClassicWorldDataset;

	struct WorldTileContainerDatasetContent
	{
		WorldTileContainerDatasetContent(AbstractDataset* parent)
			:
			parent(parent)
		{
		}

		DatasetOptional<ClassicWorldDataset> parent;
		std::vector<WorldTileDataset> tiles;
	};

	class WorldTileContainerDataset
		:
		public CommonDataset<WorldTileContainerDatasetContent>
	{
	public:
		WorldTileContainerDataset()
			:
			CommonDataset(this)
		{
		}
		
		WorldTileContainerDataset(ClassicWorldDataset* parent)
			:
			CommonDataset(this)
		{
			dataset.parent.setDataset(parent);
		}

		bool loadDynamic(Resource::ReadPipe* const pipe) override
		{
		}

		bool saveDynamic(Resource::WritePipe* const pipe) override
		{
		}

		bool loadStatic(Resource::ReadPipe* const pipe) override
		{
		}

		bool saveStatic(Resource::WritePipe* const pipe) override
		{
		}
	};

	namespace WorldTileContainerTask
	{
	}
}
