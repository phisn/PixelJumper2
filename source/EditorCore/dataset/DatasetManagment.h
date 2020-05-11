#pragma once

#include "EditorDataset.h"

#include <cassert>
#include <optional>

namespace Editor
{
	struct DatasetEntry
	{
		void* dataset;
		AbstractDataset* abstractDataset;
		int count;
	};

	class DatasetManagment
		:
		public Resource::ResourceBase
	{
		static DatasetManagment* instance;

	public:
		static DatasetManagment* Instance()
		{
			assert(instance != NULL);
			return instance;
		}

		DatasetManagment()
		{
			assert(instance == NULL);
			instance = this;
		}

		~DatasetManagment()
		{
			instance = NULL;
		}

		DatasetEntry& aquire(DatasetID datasetID)
		{
			return datasets[datasetID];
		}

		DatasetEntry& create(DatasetType type)
		{
			return create(type, Module::Random::MakeRandom<DatasetID>());
		}

		template <typename DatasetT>
		DatasetEntry& create()
		{
			return create<DatasetT>(new DatasetT);
		}

		template <typename DatasetT>
		DatasetEntry& create(DatasetT* dataset)
		{
			DatasetEntry& entry = datasets[Module::Random::MakeRandom<DatasetID>()];

			entry.abstractDataset = dataset;
			entry.count = 0;
			entry.dataset = (void*) dataset;

			return entry;
		}

		bool make(Resource::ReadPipe* const pipe) override
		{
			uint32_t datasetCount;

			if (!pipe->readValue(&datasetCount))
			{
				return false;
			}

			for (size_t i = 0; i < datasetCount; ++i)
			{
				DatasetID id;
				DatasetType type;

				if (!pipe->readValue(&id) ||
					!pipe->readValue(&type))
				{
					return false;
				}

				DatasetEntry entry = create(type, id);

				if (!entry.abstractDataset->loadStatic(pipe))
				{
					return false;
				}
			}

			for (DatasetPair& pair : datasets)
				if (!pair.second.abstractDataset->loadDynamic(pipe))
				{
					return false;
				}

			return true;
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			if (uint32_t datasetCount = datasets.size(); !pipe->writeValue(&datasetCount))
			{
				return false;
			}

			for (DatasetPair& pair : datasets)
			{
				DatasetType type = pair.second.abstractDataset->getType();

				if (!pipe->writeValue(&pair.first) ||
					!pipe->writeValue(&type) ||
					!pair.second.abstractDataset->saveStatic(pipe))
				{
					return false;
				}
			}

			for (DatasetPair& pair : datasets)
				if (!pair.second.abstractDataset->saveDynamic(pipe))
				{
					return false;
				}

			return true;
		}

	private:
		std::map<DatasetID, DatasetEntry> datasets;
		typedef decltype(datasets)::value_type DatasetPair;

		DatasetEntry& create(DatasetType type, DatasetID id)
		{
			DatasetEntry e;
			return e;
		}
	};

	template <typename DatasetT>
	class DatasetReference
	{
	public:
		DatasetReference(DatasetID id)
			:
			DatasetReference(DatasetManagment::Instance()->aquire(id))
		{
		}

		DatasetReference(DatasetEntry& entry)
			:
			entry(entry)
		{
			this->entry = entry;
		}

		~DatasetReference()
		{
			--entry.count;
		}

		DatasetReference& operator=(DatasetEntry& entry)
		{
			this->entry = entry;
		}

	private:
		DatasetEntry& entry;
	};

	template <typename DatasetT>
	class DatasetViewer
		:
		public Util::Notifier<DatasetViewer<DatasetT>>
	{
		friend class DatasetManagment;

	public:
		using Notifier::notify;

		DatasetViewer(AbstractDataset* parent)
			:
			parent(parent)
		{
		}

		DatasetT* operator->() const
		{
			assert(dataset != NULL);
			return dataset;
		}

		bool has() const
		{
			return dataset != NULL;
		}

		DatasetT* getDataset() const
		{
			return dataset;
		}

	private:
		DatasetT* dataset = NULL;
		AbstractDataset* parent;
	};
}
