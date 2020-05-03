#pragma once

#include "EditorDataset.h"

#include "FrameworkCore/imgui/ImGuiModalWindow.h"

#include <SFML/Graphics.hpp>

namespace Editor
{
	struct ClassicContextConnectionNode;
	struct ClassicContextWindowAccess
	{
		virtual void removeLink(ClassicContextConnectionNode* node) = 0;
		virtual void createLink(
			ClassicWorldDataset* source, 
			ClassicWorldDataset* target) = 0;
		virtual void beginLink(ClassicWorldDataset* dataset) = 0;
	};

	class ClassicContextPopup
		:
		public Framework::IndependentPopupWindow
	{
	public:
		ClassicContextPopup(ClassicContextWindowAccess* access)
			:
			access(access)
		{
		}

	protected:
		ClassicContextWindowAccess* access;
	};
}