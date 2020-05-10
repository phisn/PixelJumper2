#pragma once

#include "EditorDataset.h"

#include <cassert>

namespace Editor
{
	template <typename DatasetT>
	class DatasetViewer
	{
		friend class DatabaseManagment;

	public:
		DatasetViewer(AbstractDataset* parent)
			:
			parent(parent)
		{
		}

		DatasetViewer& operator->() const
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

	template <typename DatasetT>
	class DatasetReference
	{

	private:
		DatasetT* dataset;
		int counter = 0;
	};

	class DatasetContainer
	{
	public:
		template <typename DatasetT>
		DatasetContainer(DatasetT* dataset)
		{
		}

	private:

	};

	class DatasetManagment
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

		template <typename DatasetT>
		void Aquire(DatasetID datasetID)
		{
		}

	private:
		std::map<DatasetID, AbstractDataset*> datasets;
	};
}
