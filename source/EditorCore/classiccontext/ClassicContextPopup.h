#pragma once

#include "EditorCore/dataset/EditorDataset.h"

#include "FrameworkCore/imgui/ImGuiModalWindow.h"

#include <SFML/Graphics.hpp>

namespace Editor::ClassicContext
{
	struct WindowAccess
	{
		virtual void beginLink(WorldNode* node) = 0;
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