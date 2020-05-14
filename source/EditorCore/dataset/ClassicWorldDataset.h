#pragma once

#include "EditorDataset.h"
#include "WorldTileContainerDataset.h"

namespace Editor
{
	class ClassicContextDataset;
	class TransitiveDataset;

	struct ClassicWorldDatasetContent
	{
		ClassicWorldDatasetContent(AbstractDataset* parent)
			:
			parent(parent)
		{
		}

		DatasetOptional<ClassicContextDataset> parent;

		Resource::WorldID worldID;
		std::string name;

		WorldTileContainerDataset* tiles;
	};

	class ClassicWorldDataset
		:
		public CommonDataset<ClassicWorldDatasetContent>
	{
		friend class ClassicContext;

	public:
		ClassicWorldDataset()
			:
			CommonDataset(this)
		{
		}

		ClassicWorldDataset(AbstractDataset* parent)
			:
			CommonDataset(this)
		{
			dataset.parent.setDataset(parent);
		}

		/*bool make(Resource::ReadPipe* const pipe) override
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
		}*/

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

		const std::vector<TransitiveDataset*>& getTransitiveOutputs() const
		{
			return transitiveOutputs;
		}

		const std::vector<TransitiveDataset*>& getTransitiveInputs() const
		{
			return transitiveInputs;
		}

	private:
		std::vector<TransitiveDataset*> transitiveOutputs;
		std::vector<TransitiveDataset*> transitiveInputs;
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
