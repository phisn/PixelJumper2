#pragma once

#include "EditorWindow.h"

#include "ClassicContextConnectionNode.h"
#include "ClassicContextWorldNode.h"

namespace Editor::ClassicContext
{
	class RemoveWorldNodeTask
		:
		public Task
	{
	public:
		RemoveWorldNodeTask()
		{
		}
	};

	// stop storing nodes inside tasks for undo redo
	// use something like momento. make the node have a state
	// like resourcebase (pipe) to easily store the state
	// and late store it in file. stored momento + dataset = node
	class RemoveTransitiveNodeTask
		:
		public Task
	{
	public:
		RemoveTransitiveNodeTask(
			ConnectionNode* node,
			WorldNode* world0,
			WorldNode* world1)
		{
			outputWorld0 = node->getElementOutput(world0);
			outputWorld1 = node->getElementOutput(world1);

			execute();
		}

		~RemoveTransitiveNodeTask()
		{
			if (removed)
			{
				delete node;
				delete world0;
				delete world1;
			}
		}

		void undo() override
		{
			world0->addTransitiveConnection(world1, node);
			world1->addTransitiveConnection(world0, node);

			ClassicWorldDataset* dataset0 = world0->getWorld();
			ClassicWorldDataset* dataset1 = world1->getWorld();

			if (outputWorld0)
			{
				dataset0->transitive.push_back(dataset1);
				dataset1->transitive.push_back(dataset0);
			}
		}

		void redo() override
		{
			execute();
		}

	private:
		void execute()
		{
			world0->removeTransitiveConnection(node);
			world1->removeTransitiveConnection(node);

			ClassicWorldDataset* dataset0 = world0->getWorld();
			ClassicWorldDataset* dataset1 = world1->getWorld();

			if (outputWorld0)
			{
				decltype(dataset0->transitive)::iterator removal = std::find(
					dataset0->transitive.begin(),
					dataset0->transitive.end(), dataset1);

				assert(removal != dataset0->transitive.end());
				dataset0->transitive.erase(removal);
			}

			if (outputWorld1)
			{
				decltype(dataset1->transitive)::iterator removal = std::find(
					dataset1->transitive.begin(),
					dataset1->transitive.end(), dataset0);

				assert(removal != dataset1->transitive.end());
				dataset1->transitive.erase(removal);
			}
		}

		bool removed = true;

		ConnectionNode* node;
		WorldNode* world0;
		WorldNode* world1;

		bool outputWorld0;
		bool outputWorld1;
	};
}
