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
			IndependentPopupWindow(WindowFlagsStatic | ImGuiWindowFlags_AlwaysAutoResize)
		{
			active = true;
		}

		virtual bool makeWindow() override
		{
			return IndependentPopupWindow::makeWindow();
		}

	protected:
		float width;
	};
}
