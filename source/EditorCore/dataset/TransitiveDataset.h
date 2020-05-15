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
			CommonDataset(this),
			oldSource(NULL),
			oldTarget(NULL)
		{
		}

		~TransitiveDataset()
		{
		}
		
		TransitiveDataset(ClassicContextDataset* parent)
			:
			TransitiveDataset()
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

	private:
		ClassicWorldDataset* oldSource;
		ClassicWorldDataset* oldTarget;
	};

	namespace TransitiveTask
	{
	}
}
