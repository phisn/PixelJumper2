#pragma once

#include "EditorCore/dataset/EditorDataset.h"

#include "FrameworkCore/imgui/ImGuiModalWindow.h"

#include <SFML/Graphics.hpp>

namespace Editor::ClassicContext
{
	struct ConnectionNode;
	struct WindowAccess
	{
		virtual void removeLink(ConnectionNode* node) = 0;
		virtual void createConnection(
			ClassicWorldDataset* source, 
			ClassicWorldDataset* target) = 0;
		virtual void beginLink(ClassicWorldDataset* dataset) = 0;
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