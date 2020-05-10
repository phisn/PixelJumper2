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

		bool make(Resource::ReadPipe* const pipe) override
		{
			return pipe->readValue(&dataset.worldID)
				&& pipe->readString(&dataset.name)
				&& dataset.tiles->make(pipe);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeValue(&dataset.worldID)
				&& pipe->writeString(&dataset.name)
				&& dataset.tiles->save(pipe);
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
