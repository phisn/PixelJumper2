#pragma once

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/ImGuiUtil.h"
#include "FrameworkCore/ImGuiWindowComponent.h"

namespace Scene
{
	class SettingsWindow
		:
		public Framework::ImGuiWindowComponent
	{

	public:
		SettingsWindow()
			:
			ImGuiWindowComponent(WindowFlagsStatic)
		{
			title = "login_window";
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
		bool onCreate() override
		{
			darkmode = ImGui::GetStyle().Colors[ImGuiCol_Text].x == 1.f;
			
			return true;
		}

		void initialize() override
		{
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