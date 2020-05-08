#pragma once

#include "ClassicContextPopup.h"
#include "EditorDataset.h"

namespace Editor::ClassicContext
{
	class WorldPopup
		:
		public Popup
	{
		const int MaxNameSize = 16;
		const float MaxPopupWidth = 100;

	public:
		WorldPopup(
			WindowAccess* access,
			ClassicWorldDataset* dataset)
			:
			Popup(access),
			dataset(dataset),
			inputText(dataset->name)
		{
			inputText.resize(MaxNameSize);
			open();
		}

	private:
		ClassicWorldDataset* dataset;

		bool makeWindow() override
		{
			return Framework::IndependentPopupWindow::makeWindow();
		}

		void onContent() override
		{
			if (ImGui::Button("Connect to", ImVec2{ MaxPopupWidth, 0 }))
			{
				access->beginLink(dataset);
				closeWindow();
			}
		}

		std::string inputText;
	};
}
