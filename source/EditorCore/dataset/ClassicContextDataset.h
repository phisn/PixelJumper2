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
		public CommonDataset<ClassicWorldDatasetContent>
	{
	public:
	};

	namespace ClassicContextTask
	{
		class CreateTransitive
			:
			public Task<ClassicContextDatasetContent>
		{
		public:
			CreateTransitive(ClassicContextDataset* parent, std::string name = "")
			{
				transitive = new TransitiveDataset{ parent };
			}

			CreateTransitive(TransitiveDataset* transitive)
				:
				transitive(transitive)
			{
			}

			~CreateTransitive()
			{
				delete dataset;
			}

			void undo() override
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

			void redo() override
			{
				removed = false;
				dataset->transtives.push_back(transitive);
			}

		private:
			bool removed = true;
			TransitiveDataset* transitive;
		};
	}
}
