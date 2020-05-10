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

		std::vector<TransitiveDataset*> transtives;
		std::vector<ClassicWorldDataset*> worlds;
	};

	class ClassicContextDataset
		:
		public CommonDataset<ClassicContextDatasetContent>
	{
	public:
		bool make(Resource::ReadPipe* const pipe) override
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
					delete dataset;
			}

		protected:
			bool removed;

			void remove()
			{
				removed = true;
				decltype(dataset->transtives)::iterator removal = std::find(
					dataset->transtives.begin(),
					dataset->transtives.end(), transitive);

				if (removal != dataset->transtives.end())
				{
					(**removal).notify(DatasetEvent::Removed);
					dataset->transtives.erase(removal);
				}
				else
				{
					Log::Error(L"missing transitive for removal in addtransitive task undo",
						dataset->transtives.size(), L"count",
						transitive, L"transitive");
				}
			}

			void create()
			{
				removed = false;
				dataset->transtives.push_back(transitive);
			}

		private:
			TransitiveDataset* transitive;
		};

		class CreateTransitive
			:
			public ChangeTransitiveBase
		{
		public:
			CreateTransitive(ClassicContextDataset* parent)
				:
				CreateTransitive(new TransitiveDataset{ parent })
			{
			}

			CreateTransitive(TransitiveDataset* transitive)
				:
				ChangeTransitiveBase(transitive)
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

		class CreateTransitive
			:
			public ChangeTransitiveBase
		{
		public:
			CreateTransitive(TransitiveDataset* transitive)
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
