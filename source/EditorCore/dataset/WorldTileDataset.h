#pragma once

#include "DatasetManagment.h"

namespace Editor
{
	class WorldTileContainerDataset;

	struct WorldTileDatasetContent
	{
		sf::Vector2i position;
		sf::Vector2i size;
	};

	class WorldTileDataset
		:
		public CommonDataset<WorldTileDatasetContent>
	{
	public:
		WorldTileDataset(WorldTileContainerDataset* parent)
			:
			parent(parent)
		{
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

		WorldTileContainerDataset* getParent() const
		{
			return parent;
		}

	private:
		WorldTileContainerDataset* parent;
	};

	namespace WorldTileTask
	{
	}
}
