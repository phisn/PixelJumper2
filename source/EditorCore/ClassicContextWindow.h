#pragma once

#include "FrameworkCore/imgui/ImGuiGridWindow.h"

namespace Editor
{
	class ClassicContextWindow
		:
		public Framework::ImGuiGridWindow
	{
	public:
		ClassicContextWindow()
		{
			title = "ClassicContext";
		}
	};
}

