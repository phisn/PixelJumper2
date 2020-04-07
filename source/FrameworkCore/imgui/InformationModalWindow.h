#pragma once

#include "ImGuiModalWindow.h"

namespace Framework
{
	class InformationModalWindow
		:
		public IndependentModalWindow
	{
	public:
		InformationModalWindow()
			:
			IndependentModalWindow(WindowFlagsStatic)
		{
			title = "information_window";
		}

		void open(std::string text)
		{
			assert(!isActive());

			this->text = text;
			ModalWindow::open();
		}

	private:
		std::string text;

		void onContent() override
		{
			ImGui::Text(text.c_str());
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::Button("Close"))
			{
				closeWindow();
			}
		}

		void preWindow() override
		{
			Framework::ImGuiSetRelativeNextWindowPos(ImVec2(0.5f, 0.5f));
		}
	};
}
