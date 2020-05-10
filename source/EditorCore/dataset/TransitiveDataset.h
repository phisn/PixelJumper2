#pragma once

#include "ClassicWorldDataset.h"

#include "ResourceCore/component/DynamicTransitionComponent.h"

namespace Editor
{
	class ClassicContextDataset;

	struct TransitiveDatasetContent
	{
		std::string entryName;
		Resource::WorldEntryID entryID;

		ClassicWorldDataset* source;
		ClassicWorldDataset* target;
	};

	class TransitiveDataset
		:
		public CommonDataset<TransitiveDatasetContent>
	{
	public:
		TransitiveDataset(ClassicContextDataset* parent)
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

		ClassicContextDataset* getParent() const
		{
			return parent;
		}

	private:
		ClassicContextDataset* parent;
	};

	namespace TransitiveTask
	{
	}
}
