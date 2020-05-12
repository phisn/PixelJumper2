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
