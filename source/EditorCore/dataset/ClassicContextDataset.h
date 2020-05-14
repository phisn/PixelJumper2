#pragma once

#include "ClassicWorldDataset.h"
#include "TransitiveDataset.h"

#include <cassert>

namespace Editor
{
	// need to build a pipe tomorrow
	// something like all childs friend their parent
	// and all have a private write and read state
	// method which contents are specified by dataset type
	// root dataset accepts a context resource and is public
	// maybe make the world dataset read write also public

	struct ClassicStageDataset
		:
		public Dataset
	{
	};

	struct ClassicContextDatasetContent
	{
		std::string contextName;
		std::string contextDescription;

		std::vector<DatasetReference<TransitiveDataset>> transtives;
		std::vector<DatasetReference<ClassicWorldDataset>> worlds;
	};

	class ClassicContextDataset
		:
		public CommonDataset<ClassicContextDatasetContent>
	{
	public:
		/*bool make(Resource::ReadPipe* const pipe) override
		{
			return pipe->readString(&dataset.contextName)
				&& pipe->readString(&dataset.contextDescription)
				&& pipe->readVectorResource(&dataset.transtives)
				&& pipe->readVectorResource(&dataset.worlds);
		}
		
		bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeString(&dataset.contextName)
				&& pipe->writeString(&dataset.contextDescription)
				&& pipe->writeVectorResource(&dataset.transtives)
				&& pipe->writeVectorResource(&dataset.worlds);
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
	};

	namespace ClassicContextTask
	{
		class ChangeTransitiveBase
			:
			public Task<ClassicContextDatasetContent>
		{
		public:
			ChangeTransitiveBase(TransitiveDataset* transitive)
				:
				transitive(transitive)
			{
				assert(transitive != NULL);
			}

			~ChangeTransitiveBase()
			{
				if (removed)
				{
					DatasetManagment::Instance()->release(transitive->getDatasetID());
				}
			}

		protected:
			bool removed;

			void remove()
			{
				decltype(dataset->transtives)::iterator removal = std::find_if(
					dataset->transtives.begin(),
					dataset->transtives.end(), 
					[this](DatasetReference<TransitiveDataset>& transitive)
					{
						return transitive.getDataset()->getDatasetID() == this->transitive->getDatasetID();
					});

				if (removal != dataset->transtives.end())
				{
					assert(transitive->getDataset().source && transitive->getDataset().target);
					removed = true;

					DatasetManagment::Instance()->obtain<TransitiveDataset>(transitive->getDatasetID());
					(***removal).notify(DatasetEvent::Removed);

					dataset->transtives.erase(removal);
				}
				else
				{
					Log::Error(L"missing transitive for removal in addtransitive task undo",
						dataset->transtives.size(), L"count",
						transitive->getDatasetID(), L"datasetID");
				}
			}

			void create()
			{
				dataset->transtives.emplace_back(transitive->getDatasetID());
				DatasetManagment::Instance()->release(transitive->getDatasetID());
				removed = false;
			}

		private:
			TransitiveDataset* transitive;
		};

		class CreateTransitive
			:
			public ChangeTransitiveBase
		{
		public:
			CreateTransitive(TransitiveDataset* transitive)
				:
				ChangeTransitiveBase(DatasetManagment::Instance()->create(transitive))
			{
				removed = true;
			}

			void undo() override
			{
				remove();
			}

			void redo() override
			{
				create();
			}
		};

		class RemoveTransitive
			:
			public ChangeTransitiveBase
		{
		public:
			RemoveTransitive(TransitiveDataset* transitive)
				:
				ChangeTransitiveBase(transitive)
			{
				removed = false;
			}

			void undo() override
			{
				create();
			}

			void redo() override
			{
				remove();
			}
		};
	}
}
