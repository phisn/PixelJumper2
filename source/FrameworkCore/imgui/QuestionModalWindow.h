#pragma once

#include "ImGuiModalWindow.h"

#include <functional>

namespace Framework
{
	class QuestionModalWindow
		:
		public IndependentModalWindow
	{
	public:
		typedef std::function<void(bool)> Callback;

		QuestionModalWindow()
			:
			IndependentModalWindow(WindowFlagsStatic)
		{
			useActive = false;
			title = "questionWindow";
		}

		void open(Callback callback, std::string question)
		{
			assert(!isActive());

			this->callback = callback;
			this->question = question;

			ModalWindow::open();
		}

		using ModalWindow::close;

	private:
		Callback callback;
		std::string question;

		void onContent() override
		{
			ImGui::Text(question.c_str());
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::Button("Yes"))
			{
				closeWindow();
				callback(true);
			}

			ImGui::SameLine();

			if (ImGui::Button("No"))
			{
				closeWindow();
				callback(false);
			}
		}
		
		void preWindow() override
		{
			Framework::ImGuiSetRelativeNextWindowPos(ImVec2(0.5f, 0.5f));
		}
	};
}