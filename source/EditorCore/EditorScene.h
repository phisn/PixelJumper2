#pragma once

#include "classiccontext/ClassicContextWindow.h"

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/imgui/ImGuiUtil.h"
#include "FrameworkCore/imgui/ImGuiWindow.h"

namespace Editor
{
	class RootWindow
		:
		public Framework::ImGuiWindowComponent
	{
	public:
		RootWindow()
			:
			ImGuiWindowComponent(Framework::WindowFlagsOverlay)
		{
			setTitle("");
		}

		void process()
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			if (begin())
			{
				ImGui::DockSpace(
					ImGui::GetID("root_dock"), 
					ImVec2(0, 0), 
					ImGuiDockNodeFlags_PassthruCentralNode);
			}
			else
			{
				Log::Error(L"root editor window closed");
			}

			end();
			ImGui::PopStyleVar();
		}
		
	private:
		using ImGuiWindowComponent::begin;
		using ImGuiWindowComponent::end;
	};
}

namespace Scene
{
	class EditorScene
		:
		public Framework::Scene
	{
	public:
		EditorScene()
		{
		}

		void initialize() override
		{
			if (!database.initialize())
			{
				Framework::Core::PushChildScene(new Editor::EditorFailureScene(
					"Failed to open database"));

				return;
			}

			ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
			 
			Editor::ClassicContext::ClassicContextWindow* window = new Editor::ClassicContext::ClassicContextWindow(
				1);

			windowManager.makeWindow(window);
		}

		void onEvent(const sf::Event event) override
		{
			for (Editor::EditorWindow* window : windowManager.getWindows())
				window->onEvent(event);
		}
		
		void onLogic(const sf::Time time) override
		{
			rootWindow.process();

			for (Editor::EditorWindow* window : windowManager.getWindows())
				window->onProcess();
		}

		void onDraw(sf::RenderTarget* const target) override
		{
		}

	private:
		Editor::RootWindow rootWindow;

		Editor::WindowManager windowManager;
		Editor::TaskManager taskManager;
		Editor::EditorDatabase database;
	};
}
