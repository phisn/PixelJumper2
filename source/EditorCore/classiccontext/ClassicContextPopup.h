#pragma once

#include "FrameworkCore/imgui/ImGuiModalWindow.h"

#include <SFML/Graphics.hpp>

namespace Editor::ClassicContext
{
	class WorldNode;
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