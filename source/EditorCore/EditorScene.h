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
			title = "editor_root_window";
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
			ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;


			Editor::ClassicContext::WindowDataset dataset;
			dataset.classicContext = &context;
			Editor::ClassicContext::ClassicContextWindow* window = new Editor::ClassicContext::ClassicContextWindow(dataset);


			windows.push_back(window);
		}

		void onEvent(const sf::Event event) override
		{
			for (Editor::EditorWindow* window : windows)
				window->onEvent(event);
		}
		
		void onLogic(const sf::Time time) override
		{
			rootWindow.process();

			for (Editor::EditorWindow* window : windows)
				window->onProcess();
		}

		void onDraw(sf::RenderTarget* const target) override
		{
		}

	private:
		Editor::RootWindow rootWindow;
		std::vector<Editor::EditorWindow*> windows;
	};
}
