#pragma once

#include "ClassicContextConnectionNode.h"

#include "ResourceCore/component/DynamicTransitionComponent.h"

namespace Editor::ClassicContext
{
	class TransitiveNode;
	class TransitivePopup
		:
		public Popup
	{
		const int MaxNameSize = 16;
		const float MaxPopupWidth = 100;

	public:
		TransitivePopup(WindowAccess* access, TransitiveNode* node)
			:
			Popup(access),
			node(node)
		{
			open();
		}

	private:
		TransitiveNode* node;

		bool makeWindow() override
		{
			return Framework::IndependentPopupWindow::makeWindow();
		}

		void onContent() override;
	};

	class TransitiveNode
		:
		public ConnectionNode
	{
	public:
		TransitiveNode(
			WorldNode* source,
			WorldNode* target,
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
			return new TransitivePopup(access, this);
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
