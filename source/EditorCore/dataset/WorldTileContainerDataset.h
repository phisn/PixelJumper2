#pragma once

#include "WorldTileDataset.h"

namespace Editor
{
	class ClassicWorldDataset;

	struct WorldTileContainerDatasetContent
	{
		std::vector<WorldTileDataset> tiles;
	};

	class WorldTileContainerDataset
		:
		public CommonDataset<WorldTileContainerDatasetContent>
	{
	public:
		WorldTileContainerDataset(ClassicWorldDataset* parent)
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

		ClassicWorldDataset* getParent() const
		{
			return parent;
		}

	private:
		ClassicWorldDataset* parent;
	};

	namespace WorldTileContainerTask
	{
	}
}
