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

		bool make(Resource::ReadPipe* const pipe) override
		{
		}

		bool save(Resource::WritePipe* const pipe) override
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
