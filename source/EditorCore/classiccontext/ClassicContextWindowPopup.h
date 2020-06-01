#pragma once

#include "ClassicContextPopup.h"

namespace Editor::ClassicContext
{
	class WorldNode;
	class WorldPopup
		:
		public Popup
	{
		const int MaxNameSize = 16;
		const float MaxPopupWidth = 100;

	public:
		WorldPopup(WindowAccess* access, WorldNode* worldNode)
			:
			Popup(access),
			worldNode(worldNode)
		{
			open();
		}

	private:
		WorldNode* worldNode;

		bool makeWindow() override
		{
			return Framework::IndependentPopupWindow::makeWindow();
		}

		void onContent() override
		{
			if (ImGui::Button("Connect to", ImVec2{ MaxPopupWidth, 0 }))
			{
				access->beginLink(worldNode);
				closeWindow();
			}
		}
	};
}
