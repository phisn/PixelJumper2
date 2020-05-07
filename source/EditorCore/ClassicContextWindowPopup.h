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
			LazyDatasetChange datasetChange{ dataset };

			if (ImGui::InputTextEx("###classicContextPopup_Name", "Name", &inputText[0], inputText.length() + 1, ImVec2{ MaxPopupWidth, 0 }, 0))
			{
				// autotrim after first \0
				dataset->name = inputText.c_str();
				datasetChange.notify();
			}

			ImGui::Separator();

			if (ImGui::Button("Connect to", ImVec2{ MaxPopupWidth, 0 }))
			{
				access->beginLink(dataset);
				closeWindow();
			}
		}

		std::string inputText;
	};
}
