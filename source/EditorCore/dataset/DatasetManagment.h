#pragma once

#include "EditorDataset.h"

#include <cassert>
#include <optional>

namespace Editor
{
	class DatasetManagment
		:
		public Resource::ResourceBase
	{
		static DatasetManagment* instance;

		struct DatasetEntry
			:
			public Util::Notifier<DatasetEntry>
		{
			using Notifier::notify;

			void* dataset;
			AbstractDataset* abstractDataset;
			int count;
		};

	public:
		typedef Util::Notifier<DatasetEntry> Notifier;

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

		template <typename DatasetType>
		DatasetType* obtain(DatasetID datasetID)
		{
			decltype(datasets)::iterator datasetEntry = datasets.find(datasetID);
			assert(datasetEntry != datasets.end());
			++datasetEntry->second.count;
			return (DatasetType*) datasetEntry->second.dataset;
		}

		void release(DatasetID datasetID)
		{
			decltype(datasets)::iterator datasetEntry = datasets.find(datasetID);
			assert(datasetEntry != datasets.end());
			
			if (--datasetEntry->second.count == 0)
			{
				datasetEntry->second.abstractDataset->notify(DatasetEvent::Removed);
				delete datasetEntry->second.abstractDataset;
				datasets.erase(datasetEntry);
			}
			else assert(datasetEntry->second.count > 0);
		}

		Notifier& getDatasetNotifier(DatasetID datasetID)
		{
			decltype(datasets)::iterator datasetEntry = datasets.find(datasetID);
			assert(datasetEntry != datasets.end());
			return datasetEntry->second;
		}

		template <typename DatasetType, typename... Arguments>
		DatasetType* create(Arguments... arguments)
		{
			return create<DatasetType>(new DatasetType(arguments...));
		}

		template <typename DatasetType>
		DatasetType* create(DatasetType* dataset)
		{
			return create<DatasetType>(dataset, generateDatasetID());
		}

	public:
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

				DatasetEntry entry = emplace(type, id);

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

		DatasetID generateDatasetID() const
		{
			DatasetID datasetID = Module::Random::MakeRandom<DatasetID>();
			DatasetID firstID = datasetID;

			while (datasets.find(datasetID) != datasets.end())
			{
				++datasetID;
				assert(datasetID != firstID);
			}
		}

		DatasetEntry& emplace(DatasetType type, DatasetID id)
		{
			DatasetEntry e;
			return e;
		}

		template <typename DatasetT>
		DatasetEntry& emplace(DatasetT* dataset, DatasetID id)
		{
			DatasetEntry& entry = datasets[id];

			entry.abstractDataset = dataset;
			entry.count = 0;
			entry.dataset = (void*)dataset;

			return entry;
		}
	};

	template <typename DatasetType>
	class DatasetReference
	{
	public:
		DatasetReference(DatasetID id)
			:
			dataset(DatasetManagment::Instance()->obtain<DatasetType>(id))
		{
		}

		~DatasetReference()
		{
			DatasetManagment::Instance()->release(dataset->getDatasetID());
		}

		DatasetReference& operator=(DatasetID id)
		{
			DatasetManagment::Instance()->release(dataset->getDatasetID());
			dataset = DatasetManagment::Instance()->obtain<DatasetType>(id);

			return *this;
		}

		DatasetType* operator->() const
		{
			assert(dataset != NULL);
			return dataset;
		}

		DatasetType* operator*() const
		{
			return dataset;
		}

		DatasetType* getDataset() const
		{
			return dataset;
		}

	private:
		DatasetType* dataset;
	};

	template <typename DatasetType>
	class DatasetOptional
		:
		public Util::Notifier<DatasetOptional<DatasetType>>
	{
		friend class DatasetManagment;

	public:
		using Notifier::notify;

		DatasetOptional(AbstractDataset* parent)
			:
			parent(parent)
		{
		}

		DatasetOptional(
			AbstractDataset* parent, 
			AbstractDataset* dataset)
			:
			parent(parent)
		{
			setDataset(dataset);
		}

		DatasetType* operator->() const
		{
			assert(dataset != NULL);
			return dataset;
		}

		void setDataset(AbstractDataset* dataset)
		{
			listenerContainer.emplace(
				DatasetManagment::Instance()->getDatasetNotifier(dataset->getDatasetID()),
				[this]()
				{
					parent->notify(DatasetEvent::Changed);
				});
		}

		bool has() const
		{
			return dataset != NULL;
		}

		DatasetType* getDataset() const
		{
			return dataset;
		}

	private:
		DatasetType* dataset = NULL;
		AbstractDataset* parent;

		std::optional<Util::NotifyListenerContainer<DatasetManagment::Notifier>> listenerContainer;
	};
}
