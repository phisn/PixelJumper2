#pragma once

#include "ClassicContextWindow.h"

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

		void onEvent(const sf::Event event) override
		{
		}
		
		void onLogic(const sf::Time time) override
		{
			rootWindow.process();

			classicContextWindow.process();
			/*
			ImGui::Begin("test2");
			ImGui::Button("b2");
			ImGui::End();

			ImGui::Begin("test3");
			ImGui::Button("b3");
			ImGui::End();
			*/
		}

		void onDraw(sf::RenderTarget* const target) override
		{
		}

	private:
		Editor::RootWindow rootWindow;
		Editor::ClassicContextWindow classicContextWindow;
	};
}
