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
			setTitle("");
		}

		void open(std::string text, std::string button = "Close")
		{
			assert(!isActive());

			this->button = button;
			this->text = text;

			ModalWindow::open();
		}

	private:
		std::string text, button;

		void onContent() override
		{
			ImGui::Text(text.c_str());
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::Button(button.c_str()))
			{
				closeWindow();
			}
		}

		bool makeWindow() override
		{
			Framework::ImGuiSetRelativeNextWindowPos(ImVec2(0.5f, 0.5f));
			return IndependentModalWindow::makeWindow();
		}
	};
}
