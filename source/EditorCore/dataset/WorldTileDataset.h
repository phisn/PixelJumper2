#pragma once

#include "DatasetManagment.h"

namespace Editor
{
	class WorldTileContainerDataset;

	struct WorldTileDatasetContent
	{
		DatasetOptional<WorldTileContainerDataset> parent;

		sf::Vector2i position;
		sf::Vector2i size;
	};

	class WorldTileDataset
		:
		public CommonDataset<WorldTileDatasetContent>
	{
	public:
		WorldTileDataset()
			:
			CommonDataset(this)
		{
		}
		
		WorldTileDataset(WorldTileContainerDataset* parent)
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

	namespace WorldTileTask
	{
	}
}
