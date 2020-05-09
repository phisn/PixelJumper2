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
