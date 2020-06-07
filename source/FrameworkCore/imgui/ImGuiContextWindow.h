#pragma once

#include "ImGuiModalWindow.h"

namespace Framework
{
	class ContextWindow
		:
		public IndependentPopupWindow
	{
	public:
		ContextWindow()
			:
			IndependentPopupWindow(WindowFlagsStatic)
		{
			active = true;
		}

		virtual bool makeWindow() override
		{
			ImGui::SetNextWindowSize(
				ImVec2(width, 0));
			return IndependentPopupWindow::makeWindow();
		}

	protected:
		float width;
	};
}
