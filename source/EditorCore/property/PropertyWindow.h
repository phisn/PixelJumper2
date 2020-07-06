#pragma once

#include "FrameworkCore/imgui/ImGuiWindow.h"

#include "WindowManager.h"

namespace Editor
{
	class PropertyWindow
		:
		public EditorWindow,
		public Framework::ImGuiWindowComponent
	{
	public:
		virtual void onProcess() override
		{
			ImGui::Text("Hello World");
		}

		virtual void onEvent(sf::Event event) override
		{
		}

		virtual void onDatabaseEvent(DatabaseEvent event) override
		{

		}

	private:

		virtual void changeWindowIndex(int index) override
		{

		}
	};
}
