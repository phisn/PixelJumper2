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

		bool make(Resource::ReadPipe* const pipe) override
		{
		}

		bool save(Resource::WritePipe* const pipe) override
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
