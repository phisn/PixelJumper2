#pragma once

#include "EditorDataset.h"
#include "WorldTileContainerDataset.h"

namespace Editor
{
	class ClassicContextDataset;

	struct ClassicWorldDatasetContent
	{
		Resource::WorldID worldID;
		std::string name;

		WorldTileContainerDataset* tiles;
	};

	class ClassicWorldDataset
		:
		public CommonDataset<ClassicWorldDatasetContent>
	{
	public:
		ClassicWorldDataset(ClassicContextDataset* parent)
			:
			parent(parent)
		{
		}

		ClassicContextDataset* getParent() const
		{
			return parent;
		}

	private:
		ClassicContextDataset* parent;
	};

	namespace ClassicWorldTask
	{
		class ChangeName
			:
			public Task<ClassicWorldDatasetContent>
		{
		public:
			ChangeName(std::string newName)
			{
				temp = std::move(newName);
			}

			void undo() override
			{
				redo();
			}

			void redo() override
			{
				dataset->name.swap(temp);
			}

		private:
			std::string temp;
		};
	}
}
