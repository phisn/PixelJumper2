#pragma once

#include "EditorCore/dataset/EditorDataset.h"

#include "FrameworkCore/imgui/ImGuiModalWindow.h"

#include <SFML/Graphics.hpp>

namespace Editor::ClassicContext
{
	//struct ConnectionNode;
	struct WindowAccess
	{
	};

	class Popup
		:
		public Framework::IndependentPopupWindow
	{
	public:
		Popup(WindowAccess* access)
			:
			access(access)
		{
		}

	protected:
		WindowAccess* access;
	};
}