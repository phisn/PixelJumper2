#pragma once

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/imgui/ImGuiUtil.h"
#include "FrameworkCore/imgui/ImGuiWindow.h"

namespace Scene
{
	class SettingsWindow
		:
		public Framework::ImGuiWindowComponent
	{

	public:
		SettingsWindow()
			:
			ImGuiWindowComponent(Framework::WindowFlagsStatic)
		{
			setTitle("login_window")
		}

		bool begin() override
		{
			Framework::ImGuiSetRelativeNextWindowPos(ImVec2(0.5f, 0.5f));

			if (ImGuiWindowComponent::begin())
			{
				return true;
			}

			return false;
		}

		void end() override
		{
			ImGuiWindowComponent::end();
		}
	};

	class SettingsScene
		:
		public Framework::Scene
	{
	public:
		void initialize() override
		{
			darkmode = ImGui::GetStyle().Colors[ImGuiCol_Text].x == 1.f;
		}

		void onEvent(const sf::Event event) override
		{
		}

		void onLogic(const sf::Time time) override
		{
			if (!window.begin())
			{
				window.end();
				return;
			}

			if (ImGui::Checkbox("Darkmode", &darkmode))
			{
				if (darkmode)
				{
					Framework::ImGuiApplyColorStyleDark();
				}
				else
				{
					Framework::ImGuiApplyColorStyleLight();
				}
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::Button("Done"))
			{
				Framework::Core::PopScene();
			}

			window.end();
		}

		void onDraw(sf::RenderTarget* const target) override
		{
		}

		void onShow() override
		{
		}

		void onHide() override
		{
		}

	private:
		bool darkmode;

		SettingsWindow window;
	};
}