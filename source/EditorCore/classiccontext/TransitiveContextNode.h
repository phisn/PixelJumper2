#pragma once

#include "ClassicContextConnectionNode.h"

#include "ResourceCore/component/DynamicTransitionComponent.h"

namespace Editor::ClassicContext
{
	class TransitiveNode
		:
		public ConnectionNode
	{
	public:
		TransitiveNode(
			ConnectionElement* source,
			ConnectionElement* target,
			Resource::WorldEntryID id,
			std::string name)
			:
			ConnectionNode(
				source,
				target),
			id(id),
			name(name)
		{
		}

		void update()
		{
		}

		Framework::IndependentPopupWindow* createPopupWindow(WindowAccess* access) override
		{
		}

		Resource::WorldEntryID getID() const
		{
			return id;
		}

	private:
		Resource::WorldEntryID id;
		std::string name;
	};
}
