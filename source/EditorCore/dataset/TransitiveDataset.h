#pragma once

#include "ClassicWorldDataset.h"

#include "ResourceCore/component/DynamicTransitionComponent.h"

namespace Editor
{
	class ClassicContextDataset;

	struct TransitiveDatasetContent
	{
		TransitiveDatasetContent(AbstractDataset* parent)
			:
			parent(parent),
			source(parent),
			target(parent)
		{
		}

		DatasetOptional<ClassicContextDataset> parent;

		std::string entryName;
		Resource::WorldEntryID entryID;

		DatasetOptional<ClassicWorldDataset> source;
		DatasetOptional<ClassicWorldDataset> target;
	};

	class TransitiveDataset
		:
		public CommonDataset<TransitiveDatasetContent>
	{
	public:
		TransitiveDataset()
			:
			CommonDataset(this)
		{
		}
		
		TransitiveDataset(ClassicContextDataset* parent)
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

	namespace TransitiveTask
	{
	}
}
