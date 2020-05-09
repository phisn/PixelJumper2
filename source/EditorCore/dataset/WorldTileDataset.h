#pragma once

#include "EditorDataset.h"

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
